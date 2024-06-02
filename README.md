# Monitoring Weather

- Hardware: ESP32 and DHT11 (DHT22)

  ![image](https://github.com/nhattruong05022003/MonitoringWeather/assets/145974955/a7bee44f-f8f5-4f81-895b-72b26748a8a6)

- How to setup:
  + Create Telegram Bot:
    1. Dowload Telegram
    2. Search BotFather

       ![image](https://github.com/nhattruong05022003/MonitoringWeather/assets/145974955/95c0ce9b-e5bc-4104-b133-30bd2416d9d7)

    3. Click on the start button and choose /newbot

       ![image](https://github.com/nhattruong05022003/MonitoringWeather/assets/145974955/c2fbb69f-a97a-40ea-9020-d5015b8c9dcb)

       Give your bot a name and username.
       
    4. If your bot is successfully created, you’ll receive a message with a link to access your newly created bot and the bot token. Bot token is a unique id that we will be using later to communicate with the bot.

      ![image](https://github.com/nhattruong05022003/MonitoringWeather/assets/145974955/e9cce325-625c-4eaa-8698-ec6cf74d7ad0)
    
  + Grab your chat ID:
    1. In your Telegram account, search for “IDBot”.

       ![image](https://github.com/nhattruong05022003/MonitoringWeather/assets/145974955/0e03d118-3763-4526-bd91-a6dc4b8dfd9d)

    2. Start a conversation with that bot and type /getid. You will get a reply back with your user ID. 

       ![image](https://github.com/nhattruong05022003/MonitoringWeather/assets/145974955/ff702fba-5dc7-4385-afe6-293919362d73)

    3. Note the user ID.
  + Get API Key from OpenWeather:
    1. Go to OpenWeather page.
    2. Create your account.
    3. Confirm your email.
    4. Wait for your API Key to be created (It usually takes 20 - 30 minutes).
    5. Go to My API Keys to receive your key.
       
       ![image](https://github.com/nhattruong05022003/MonitoringWeather/assets/145974955/8d120e5b-4b11-4ce8-b6e3-be096875bdb8)

  + Get your latitude and longtitude:
    1. Go to Google map.
    2. Choose the place you want to get latitude and longtitude. Double click to mark the point.
    3. Look at the page url you will see the place's latitude and longtitude.
   
       ![Untitled](https://github.com/nhattruong05022003/MonitoringWeather/assets/145974955/1edfbacf-ab78-4668-a82a-b962cf3da247)

      Note: latitude first and then longtitude.
    
  NOTE ALL THE INFORMATION INTO YOUR FAVOURITE TEXT EDITOR.
  
- How to use:
  + Power up your MCU and sensor.
  + Wait a few seconds, you will see this wifi pops up.
 
    ![image](https://github.com/nhattruong05022003/MonitoringWeather/assets/145974955/fc0347ee-84fe-4795-90ca-689ef4b74292)

    Connect it.
  + Go to your webrowser and type 192.168.4.1. A website will show up.
 
    ![image](https://github.com/nhattruong05022003/MonitoringWeather/assets/145974955/28509d00-90bb-4094-8825-653c891eda88)

  + Click to Configure Wifi. It will direct you to the configuration page.
    
  + Type all the information here
 
    ![image](https://github.com/nhattruong05022003/MonitoringWeather/assets/145974955/b7177768-8bcd-4d01-919e-3d6135dc2e87)

  + Click save.
  + Wait a few seconds for the MCU to connect the wifi.

DONE ! NOW YOU CAN MONITORING YOUR LOCAL AND CITY WEATHER VIA YOUR TELEGRAM BOT.

NOTE1: You can go to https://openweathermap.org/current, find the API Call to check the url of the OpenWeatherMap json file. If it doesn't look like the one in the code. Just change it.

NOTE2: If you want to change the configuration uncomment wm.resetSettings(), burn the code, follow "How to use", when ESP successfully connect to the wifi, comment wm.resetSettings(), burn the code again and done.
