#!/bin/sh

[ "$1" != -s ] && while true; do nc -lp 9034 -c "exec $0 -s" ; done

while read line; do
	sleep .2
	case "$line" in
	'GETO InternetGatewayDevice.')
		cat <<-EOF
			InternetGatewayDevice.User.1
			InternetGatewayDevice.User.2
		EOF
		;;
	'GETD InternetGatewayDevice. 0')
		cat <<-EOF
			InternetGatewayDevice.DeviceSummary;InternetGatewayDevice:1.2[](Baseline:1,EthernetLAN:1,WiFiLAN:1,ADSLWAN:1,EthernetWAN:1,QoS:1,QoSDynamicFlow:1,Bridging:1,Time:1,IPPing:1,TraceRoute:1,DeviceAssociation:1,UDPConnReq:1,USBHostBasic:1,SelfTestDiag:1),VoiceService:1.0[1](Endpoint:1,TAEndpoint:1,SIPEndpoint:1)InternetGatewayDevice:2.2[](RouterAdvertisement:1,)InternetGatewayDevice:2.2[]
			InternetGatewayDevice.LANDeviceNumberOfEntries;3
			InternetGatewayDevice.WANDeviceNumberOfEntries;3
			InternetGatewayDevice.UserNumberOfEntries;2
		EOF
		;;
	'GETO Device.')
		cat <<-EOF
			Device
			Device.WiFi
			Device.Routing
			Device.Firewall
			Device.WiFi
			Device.WiFi.X_ADB_APScan
			Device.WiFi.Radio.1
			Device.WiFi.SSID.1
			Device.WiFi.SSID.2
			Device.WiFi.SSID.3
			Device.WiFi.SSID.4
			Device.WiFi.AccessPoint.1
			Device.WiFi.AccessPoint.10
			Device.WiFi.AccessPoint.2
			Device.WiFi.AccessPoint.3
			Device.WiFi.AccessPoint.12
			Device.Routing
			Device.Routing.Router.1
			Device.Routing.RouteInformation
			Device.Routing.RIP
			Device.Firewall
			Device.Firewall.Chain.1
			Device.Firewall.Chain.2
			Device.Firewall.Chain.10
			Device.Firewall.Chain.12
		EOF
		;;
	'GETD Device. 0')
		cat <<-EOF
			Device.InterfaceStackNumberOfEntries;53
		EOF
		;;
	'GETD Device.WiFi. 0')
		cat <<-EOF
			Device.WiFi.AccessPointNumberOfEntries;4
			Device.WiFi.X_ADB_WDSNumberOfEntries;0
			Device.WiFi.RadioNumberOfEntries;1
			Device.WiFi.SSIDNumberOfEntries;4
		EOF
		;;
	'GETD Device.Routing. 0')
		cat <<-EOF
			Device.Routing.RouterNumberOfEntries;0
		EOF
		;;
	'GETD Device.Firewall. 0')
		cat <<-EOF
			Device.Firewall.Config;Advanced
			Device.Firewall.LastChange;2009-01-01T01:01:31+01:00
			Device.Firewall.Version;
			Device.Firewall.Enable;true
			Device.Firewall.AdvancedLevel;Device.Firewall.Level.1
			Device.Firewall.Type;Stateful
			Device.Firewall.LevelNumberOfEntries;1
			Device.Firewall.ChainNumberOfEntries;20
		EOF
		;;
	GETD*)
		line=${line%%. 0}
		line=${line##*.}
		echo "Leave;$line"
		;;
	SET*|DEL*)
		cat <<-EOF
			OK
			OK
		EOF
		;;
	ADD*)
		echo $(($(date +%s) % 10 + 10))
		;;
	BYE)
		exit
		;;
	*)
		echo '#ERROR #2'
		;;
	esac
	printf '\0'
done
