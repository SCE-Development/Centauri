# Centauri


### Table of contents
- [Project Goal](#project-goal)
- [Tools & Technologies](#tools-and-technologies)
- [Project Architecture](#project-architecture)
- [Getting Started](#getting-started)

<hr/>

### Project Goal
To implement an RFID door lock access system in the SCE club room's officer's room

<hr/>

### Tools and Technologies

|  ESP32 |  Arduino |  Node JS |  React JS |  Express JS| AWS IOT | MongoDB |
|---|---|---|---|---|---|---|

<hr/>

### Project Architecture
![centauri-model](https://user-images.githubusercontent.com/75186517/156943979-f7d90395-dc03-4df7-9f45-d46dd02057a8.png)

<hr/>


### Getting Started 

Prerequisites: Have AWS-IOT.zip downloaded and extracted (Found in SCE shared Google Drive folder)

 1. Create a new file in `secrets\` directory called `secrets.h`
 2. Copy paste the contents of `secrets.example.h` into `secrets.h`
 3. Open all certificates and private keys in a text editor
 4. Copy paste the content of each certificate and key file into the corresponding field in `secrets.h` file created earlier
 5. Change WiFi name and password properties in `secrets.h`
 6. Flash it to ESP32