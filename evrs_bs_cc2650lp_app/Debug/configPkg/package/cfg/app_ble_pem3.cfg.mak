# invoke SourceDir generated makefile for app_ble.pem3
app_ble.pem3: .libraries,app_ble.pem3
.libraries,app_ble.pem3: package/cfg/app_ble_pem3.xdl
	$(MAKE) -f C:\ti\ble_sdk_2_02_02_25\ble_examples\examples\cc2650lp\simple_central\ccs\config/src/makefile.libs

clean::
	$(MAKE) -f C:\ti\ble_sdk_2_02_02_25\ble_examples\examples\cc2650lp\simple_central\ccs\config/src/makefile.libs clean

