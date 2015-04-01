#!/bin/sh

[ "$1" != -s ] && while true; do nc -lp 9034 -c "exec $0 -s" ; done

while read line; do
	sleep .2
	case "$line" in
	'GETO Device.+')
		cat <<-EOF
			Device
			Device.WiFi
			Device.Routing
			Device.Firewall
		EOF
		;;
	'GETD Device. 0')
		cat <<-EOF
			Device.InterfaceStackNumberOfEntries;53
		EOF
		;;
	'GETO Device.WiFi.+')
		cat <<-EOF
			Device.WiFi
			Device.WiFi.X_ADB_APScan
			Device.WiFi.Radio.1
			Device.WiFi.SSID.1
			Device.WiFi.SSID.2
			Device.WiFi.SSID.3
			Device.WiFi.SSID.4
			Device.WiFi.AccessPoint.1
			Device.WiFi.AccessPoint.2
			Device.WiFi.AccessPoint.3
			Device.WiFi.AccessPoint.4
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
	'GETO Device.Routing.+')
		cat <<-EOF
			Device.Routing
			Device.Routing.Router.1
			Device.Routing.RouteInformation
			Device.Routing.RIP
		EOF
		;;
	'GETD Device.Routing. 0')
		cat <<-EOF
			Device.Routing.RouterNumberOfEntries;0
		EOF
		;;
	'GETO Device.Firewall.+')
		cat <<-EOF
			Device.Firewall
			Device.Firewall.Chain.1
			Device.Firewall.Chain.2
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
	'BYE')
		exit
		;;
	*)
		echo '#ERROR #2'
		;;
	esac
done
