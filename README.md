# INF2004 - Embedded Systems Project
**I. Introduction**

In the realm of cybersecurity, the ability to understand and counter potential threats is paramount. This project embarks on a journey through a series of interconnected processes, forming a comprehensive flow designed to explore vulnerabilities in wireless networks. The focal points of this endeavor include scanning for available Wi-Fi networks, assuming the identity of a chosen network by creating a deceptive Access Point (AP), manipulating DNS settings, and engaging in social engineering tactics to simulate a Google login page. This intricate sequence is augmented by the capability to store captured credentials securely on an external SD card, providing a holistic examination of potential security loopholes.

**II. Key Features**

- Wifi Scanning
- Spoofing WiFi to an Access Point (AP)
- DNS Spoofing
- Social Engineer the Google Page 
- Save Data to a SD Card (E.g. )

**III. Objectives**

The primary objective of this project is to demonstrate a controlled and ethical exploration of security vulnerabilities within wireless networks. By following a systematic flow, the project aims to showcase the intricacies involved in scanning for Wi-Fi networks, manipulating network infrastructure by creating a deceptive AP, and employing DNS spoofing techniques. Furthermore, the project ventures into the realm of social engineering by simulating a Google login page, illustrating how attackers might exploit trust to gain unauthorized access. The integration of an SD card for secure data storage adds an additional layer of realism to the exploration, mimicking potential scenarios where captured credentials are stored for further analysis. Through these simulated processes, the project seeks to raise awareness about cybersecurity threats and foster a better understanding of potential countermeasures to protect against them.

**IV. Overview of Program (Modular Design)**

1. **Wi-Fi Scanning Module**:

- Purpose: Identifies and lists available Wi-Fi networks in the vicinity.
- Functionality: Utilizes scanning techniques to discover and present a list of accessible Wi-Fi networks.

2. **AP Spoofing Module**:

- Purpose: Creates a deceptive Access Point (AP) to imitate a chosen Wi-Fi network.
- Functionality: Spoofs the identity of a selected Wi-Fi network, enticing devices to connect to the deceptive AP.

3. **DNS Spoofing Module**:

- Purpose: Manipulates Domain Name System (DNS) settings to redirect traffic surreptitiously.
- Functionality: Intercepts DNS requests and provides fraudulent responses, redirecting users to specified destinations.

4. **Social Engineering Module**:

- Purpose: Simulates a Google login page to trick users into revealing credentials.
- Functionality: Craftily presents a deceptive login interface, exploiting trust to capture username and password information.

5. **Data Storage Module (SD Card)**:

- Purpose: Securely stores captured credentials for later analysis.
- Functionality: Writes and reads data to and from an external SD card, ensuring the safe storage of acquired information.

**V. Requirements to Run**

- lwIP 

**VI. How to run the program?**

Steps:
1. Connect to Pico WH
2. Scan for nearby WiFi
3. Spoofed one of the scanned WiFi and act as an AP
4. It will then lead to the WiFi Login page
5. User will then choose to login via Gmail account
6. The login credentials will then be stored in the SD Card

**VII. Citations**

1. https://github.com/LearnEmbeddedSystems/pico-w-webserver-template for the original code to set up a web server
2. https://github.com/krzmaz/pico-w-webserver-example for the part on displaying the received IP address for the Pico W

**VIII. Task Allocations**

| Tasks  | Name |
| ------------- | ------------- |
| Wifi Scanning  | Content Cell  |
| Spoofing WiFi to an Access Point (AP)  | Content Cell  |
| DNS Spoofing  | Content Cell  |
| Web Server | Jia  Yi |
| Save Data to a SD Card (E.g. )  | Content Cell  |
