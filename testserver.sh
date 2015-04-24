#!/bin/sh

[ "$1" != -s ] && while true; do nc -lp 9034 -c "exec $0 -s" ; done

geto() {
	case "$@" in
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
	'GETO InternetGatewayDevice.')
		cat <<-EOF
			InternetGatewayDevice.User.1
			InternetGatewayDevice.User.2
		EOF
		;;
	*)	echo '#ERROR 3'
	esac
}

getd() {
	case "$@" in
	'GETD InternetGatewayDevice. 0')
		cat <<-EOF
			InternetGatewayDevice.DeviceSummary;InternetGatewayDevice:1.2[](Baseline:1,EthernetLAN:1,WiFiLAN:1,ADSLWAN:1,EthernetWAN:1,QoS:1,QoSDynamicFlow:1,Bridging:1,Time:1,IPPing:1,TraceRoute:1,DeviceAssociation:1,UDPConnReq:1,USBHostBasic:1,SelfTestDiag:1),VoiceService:1.0[1](Endpoint:1,TAEndpoint:1,SIPEndpoint:1)InternetGatewayDevice:2.2[](RouterAdvertisement:1,)InternetGatewayDevice:2.2[]
			InternetGatewayDevice.LANDeviceNumberOfEntries;3
			InternetGatewayDevice.WANDeviceNumberOfEntries;3
			InternetGatewayDevice.UserNumberOfEntries;2
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
	*)
		line=${line%%. 0}
		line=${line##*.}
		echo "Leave;$line"
		;;
	esac
}

getmdp() {
	case "$@" in
	'GETMDP Device. 0')
		cat <<EOF
Device.
	access:ro
	range:1..1
	inh_na:-1
Device.UPnP.
	access:rw
	range:0..*
	inh_na:-1
Device.WiFi.
	access:ro
	range:1..1
	inh_na:-1
Device.InterfaceStackNumberOfEntries
	access:ro
	type:unsignedInt
	default:0
	inh_na:-1
	exprt:1
EOF
		;;
	'GETMDP Device.WiFi. 0')
		cat <<EOF
Device.WiFi.
	access:ro
	range:1..1
	inh_na:-1
Device.WiFi.X_ADB_APScan.
	access:ro
	range:1..1
	inh_na:-1
Device.WiFi.Radio.1.
	access:ro
	range:0..*
	inh_na:-1
Device.WiFi.AccessPointNumberOfEntries
	access:ro
	type:unsignedInt
	default:0
	inh_na:-1
	exprt:1
Device.WiFi.X_ADB_WDSNumberOfEntries
	access:ro
	type:unsignedInt
	default:0
	inh_na:-1
	exprt:1
Device.WiFi.RadioNumberOfEntries
	access:ro
	type:unsignedInt
	default:0
	inh_na:-1
	exprt:1
Device.WiFi.SSIDNumberOfEntries
	access:ro
	type:unsignedInt
	default:0
	inh_na:-1
	exprt:1
EOF
		;;
	'GETMDP Device.Firewall. 0')
		cat <<EOF
Device.Firewall.
	access:ro
	range:1..1
	inh_na:-1
Device.Firewall.LastChange
	access:ro
	type:dateTime
	default:0001-01-01T00:00:00Z
	inh_na:-1
	exprt:1
Device.Firewall.Version
	access:ro
	type:string:size:0..16
	inh_na:-1
	exprt:1
Device.Firewall.Enable
	access:rw
	type:boolean
	default:true
	inh_na:-1
	exprt:1
Device.Firewall.AdvancedLevel
	access:rw
	type:string:size:0..256
	pathref:targetParent:.Firewall.Level.
	pathref:targetType:6
	pathref:refType:1
	default:
	inh_na:-1
	exprt:1
Device.Firewall.Type
	access:ro
	type:string
	enum:Stateful
	enum:Stateless
	default:Stateful
	inh_na:-1
	exprt:1
EOF
		;;
	*)	echo '#ERROR 3'
	esac
}

while read line; do
	sleep .2
	case "$line" in
	GETO*)
		geto "$line"
		;;
	GETD*)
		getd "$line"
		;;
	GETMDP*)
		getmdp "$line"
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
	SAVE|RESET)
		echo OK
		;;
	*)
		echo '#ERROR #2'
		;;
	esac
	printf '\0'
done
