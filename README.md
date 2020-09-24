# Use Azure MXChip board with Ultrasonic Range Finder and Stream Analytics to Present Data in Power BI


In this tutorial, you'll learn how to connect MXChip Board(AZ3166) with Ultrasonic Range Finder sensor, cleanse data using ML of Stream Analytics, and display it in Power BI in a few simple steps.

In order to process and sanitize sonar sensor's data, which often comes with spikes or deeps, we are using on-board processing(by selecting an average of N number of measurements in the defined range) and ML algorithm, provided by Stream Analytics - [AnomalyDetection_SpikeAndDip ](https://docs.microsoft.com/en-us/stream-analytics-query/anomalydetection-spikeanddip-azure-stream-analytics). This OOB algorithm designed to catch temporary anomalies in the collection of events.

The below GIF shows real-time stream data in Power BI. The green line represents sensor data, and black line shows data processed by Stream Analytics. As you can see, AnomalyDetection_SpikeAndDip algorithm handled abnormal spikes and deeps very nicely.

![](http://borisbrodsky.com/wp-content/uploads/2020/07/asa_final-1.gif)


## You'll need:
1. [Azure DevKit](https://microsoft.github.io/azure-iot-developer-kit/)
2. Sonar Sensor. You'll need a sonar with analog output. I used [LV-MaxSonar-EZ1 Ultrasonic Range Finder](https://www.amazon.com/Maxbotix-MB1010-LV-MaxSonar-EZ1-Ultrasonic-Finder/dp/B00A7YGVJI)
3. Azure Account and a small budget to run Stream Analytics.
4. Breakout Board Adapter for AZ3166. I used [Keyestudio ](https://www.amazon.com/gp/product/B078NW5GZ8) board, but few other boards are available.
5. Visual Studio Code with number of extensions. Please check this [guide](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-arduino-iot-devkit-az3166-get-started) for additional details
##  Hardware

![](http://borisbrodsky.com/wp-content/uploads/2020/07/board_map.png)

Connect breakout board to your sonar sensor as shown on the above diagram. I selected to use Analog input 5 that maps to connector 4 on Keyestudio board.

##  Software
In this tutorial, we will create three Azure components: IoT Hub, Stream Analytics, and Power BI. We will deploy the provided code on az3166 device using VS Code.

![](http://borisbrodsky.com/wp-content/uploads/2020/07/AzureIoT-e1595255244489.png)

###  Deploy Code and Provision Azure IoT Hub

1. Update your device to the latest firmware version and connect it to the internet. Please follow this [link](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-arduino-iot-devkit-az3166-get-started).
2. Open VS Code, click F1 and click Git: Clone, select this repository: https://github.com/brodbor/mxchip-inventory-managment.git
3. In VS Code, click F1 and start typing "Provision Azure Services", select "Azure IoT Device Workbench: Provision Azure Services....". Follow  the guide to complete IoT Hub and Device provisioning 


![](http://borisbrodsky.com/wp-content/uploads/2020/07/iot-config-3.gif )

4. Now that you have IoT Hub provisioned, you need to configure connection strig on the device. click F1 and start typing "Azure IoT Device Workbench: Configure Device Settings". Select Config Device Connection String > Select IoT Hub Device Connection String.
5. Click F1 and select "Azure IoT Device Workbench: Upload Device Code"
6. Test Deployment. Expand AZURE IOT HUB node, right-click on az3166 device and select "Start Monitoring build-in Event Endpoint"

![](http://borisbrodsky.com/wp-content/uploads/2020/07/test_iot_hub-1024x706.png )


### Provision Stream Analytics 

1. Navigate to "IoTHubInput.json" file, under the StreamAnalytics folder and update its content as shown below. Select details of recently provisioned IoT Hub.

![](http://borisbrodsky.com/wp-content/uploads/2020/07/asa-configuration1-3.gif)

2. login to powerbi.com and select Workplaces and click on create new.
3. Configure Power BI output. Open "powerBIoutput.json" file and update it with your Power BI details

![](http://borisbrodsky.com/wp-content/uploads/2020/07/board_map.png)

4. Deploy Stream Analytics to Azure subscription. Click F1 and type "**ASA: Submit to Azure**". Follow guided steps to provision the instance.
5. Test deployment by executing Job locally. Navigate to the .asaql file, right-click and select "ASA: Start Local Run"

![](http://borisbrodsky.com/wp-content/uploads/2020/07/asa_test.gif)

### Configure Power BI

1. Navigate to powerbi.com and select your Workspace. Navigate to Datasets and confirm that AzureIoTDataset is created.
2. Navigate to Dashboards and click + on the upper part of the screen. Create new dashboard.
3. Enter newly created dashboard and click + Add tile. Select Custom Streaming Data 

![](http://borisbrodsky.com/wp-content/uploads/2020/07/powerbi-data-stream-1024x660.png)

4. Select Streaming dataset from the list and create streaming tile as shown:

![](http://borisbrodsky.com/wp-content/uploads/2020/07/powerbi_tile-765x1024.png)


Done!
