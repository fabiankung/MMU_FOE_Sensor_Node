![Sensornode_V0_99](./FOE_SensorNode_V0_99.jpg)

Arduino:
1. Sample sourcecode to implement transmitter (TX) unit and receiver (RX) unit.
2. The 'basic' version is for one-to-one, e.g. one TX unit to one RX unit using similar address value.
3. The 'multi' version is for many-to-one, e.g. up to 6 TX units to one RX unit using 6 addresses with nRF24L01+ logical datapipe method.
4. The software version with 'sleep' added to the suffix indicates low-power operation. The processor and NRF24L01+ radio will be power up
   every few seconds, transmit a packet of data then the processor and radio will go back to sleep mode. 

Visual Studio:
1. A demonstration of host software written in Visual Basic .NET. The software is archived as Microsoft Visual Studio Project Template. You can use Microsoft Visual Studio to unarchive it and then compile into executable codes.
