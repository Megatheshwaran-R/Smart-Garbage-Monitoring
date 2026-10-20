% ThingSpeak Channel Configuration
channelID = 0000000; % Replace with your ThingSpeak Channel ID

% ThingSpeak Alerts API Key (Keep this secret)
alertApiKey = 'YOUR_THINGSPEAK_ALERT_API_KEY';

alertUrl = "https://api.thingspeak.com/alerts/send";
options = weboptions("HeaderFields", ["ThingSpeak-Alerts-API-Key", alertApiKey]);
alertSubject = "Smart Waste Management Alert: Bin Status";

% Read the recent 30 days of data from field 1
garbageData = thingSpeakRead(channelID, 'NumDays', 30, 'Fields', 1);

if isempty(garbageData)
    alertBody = 'No telemetry data received from the smart waste bin.';
else
    span = max(garbageData) - min(garbageData);
    thresholdValue = 0.1 * span + min(garbageData);
    lastValue = garbageData(end);

    if (lastValue <= thresholdValue)
        alertBody = 'Notice: Garbage bin level has reached 0% (Empty).';
    else
        alertBody = 'Alert: Garbage bin capacity threshold reached (>90% full). Dispatch collection team.';
    end
end

% Send Email Notification
try
    webwrite(alertUrl, "body", alertBody, "subject", alertSubject, options);
    fprintf("Alert sent successfully.\n");
catch someException
    fprintf("Failed to send alert: %s\n", someException.message);
end
