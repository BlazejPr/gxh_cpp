


 
XCC     = g++
XCC2    = gcc
LD      = g++





 
CFLAGS  = -c -Wall -g  -march=armv7-a -mtune=cortex-a8 -mcpu=cortex-a8  -fPIC  -fpermissive -Wwrite-strings

LDFLAGS = -g -ldl  -lpthread -I. -L. -o 
SHARED  = -shared -o  


 



all: gxh
 
 
# ----------- sql lite3  bez dynamicznych bibliotek-----------------------------------------	 

shell.o: shell.c
	$(XCC2) $(CFLAGS) -DSQLITE_THREADSAFE=2 shell.c -o $@

sqlite3.o: sqlite3.c
	$(XCC2) $(CFLAGS) -DSQLITE_THREADSAFE=2 sqlite3.c -o $@


# ------------------- core aplication ------------------------------------------

gxh_structure.o: gxh_structure.cpp 
	$(XCC) $(CFLAGS) gxh_structure.cpp -o $@	 

main.o: main.cpp 
	$(XCC) $(CFLAGS) main.cpp -o $@	 
	
gxh_scope.o: gxh_scope.cpp 
	$(XCC) $(CFLAGS) gxh_scope.cpp -o $@	 	
	
gxh_core.o: gxh_core.cpp 
	$(XCC) $(CFLAGS) gxh_core.cpp -o $@		

app_zi_queue.o: app_zi_queue.cpp 
	$(XCC) $(CFLAGS) app_zi_queue.cpp -o $@	
	
app_zc_queue.o: app_zc_queue.cpp 
	$(XCC) $(CFLAGS) app_zc_queue.cpp -o $@		
	
	
	
	
app_port.o: app_port.cpp 
	$(XCC) $(CFLAGS) app_port.cpp -o $@		
	
app_port_ethernet.o: app_port_ethernet.cpp 
	$(XCC) $(CFLAGS) app_port_ethernet.cpp -o $@	
	
app_port_serial.o: app_port_serial.cpp 
	$(XCC) $(CFLAGS) app_port_serial.cpp -o $@		
 		
	
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------

h_sunset.o: h_sunset.cpp 
	$(XCC) $(CFLAGS) h_sunset.cpp -o $@ 

#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------

app_modbus_rtu.o: app_modbus_rtu.cpp 
	$(XCC) $(CFLAGS) app_modbus_rtu.cpp -o $@ 

app_driver.o: app_driver.cpp 
	$(XCC) $(CFLAGS) app_driver.cpp -o $@ 
	
app_driver_techbase.o: app_driver_techbase.cpp 	 
	$(XCC) $(CFLAGS) app_driver_techbase.cpp -o $@ 
	$(XCC) $(SHARED) app_driver_techbase.so $@ gxh_scope.o app_driver.o gxh_structure.o sqlite3.o	

app_driver_elpiast.o: app_driver_elpiast.cpp 	 
	$(XCC) $(CFLAGS) app_driver_elpiast.cpp -o $@ 
	$(XCC) $(SHARED) app_driver_elpiast.so $@ gxh_scope.o app_driver.o gxh_structure.o sqlite3.o app_modbus_rtu.o	
	
app_driver_provent.o: app_driver_provent.cpp 	 
	$(XCC) $(CFLAGS) app_driver_provent.cpp -o $@ 
	$(XCC) $(SHARED) app_driver_provent.so $@ gxh_scope.o app_driver.o gxh_structure.o sqlite3.o app_modbus_rtu.o
	
	
app_driver_satel.o: app_driver_satel.cpp 	 
	$(XCC) $(CFLAGS) app_driver_satel.cpp -o $@ 
	$(XCC) $(SHARED) app_driver_satel.so $@ gxh_scope.o app_driver.o gxh_structure.o sqlite3.o app_modbus_rtu.o	

app_driver_netronix.o: app_driver_netronix.cpp 	 
	$(XCC) $(CFLAGS) app_driver_netronix.cpp -o $@ 
	$(XCC) $(SHARED) app_driver_netronix.so $@ gxh_scope.o app_driver.o gxh_structure.o sqlite3.o app_modbus_rtu.o

#-------------------------------------------------------------------------------

app_module.o: app_module.cpp 
	$(XCC) $(CFLAGS) app_module.cpp -o $@ 

app_module_light.o: app_module_light.cpp 	 
	$(XCC) $(CFLAGS) app_module_light.cpp -o $@ 
	$(XCC) $(SHARED) app_module_light.so $@ gxh_scope.o app_module.o gxh_structure.o sqlite3.o
	
app_module_techbase.o: app_module_techbase.cpp 	 
	$(XCC) $(CFLAGS) app_module_techbase.cpp -o $@ 
	$(XCC) $(SHARED) app_module_techbase.so $@ gxh_scope.o app_module.o gxh_structure.o sqlite3.o	
	
app_module_elpiast.o: app_module_elpiast.cpp 	 
	$(XCC) $(CFLAGS) app_module_elpiast.cpp -o $@ 
	$(XCC) $(SHARED) app_module_elpiast.so $@ gxh_scope.o app_module.o gxh_structure.o sqlite3.o	
	
app_module_blind.o: app_module_blind.cpp 	 
	$(XCC) $(CFLAGS) app_module_blind.cpp -o $@ 
	$(XCC) $(SHARED) app_module_blind.so $@ gxh_scope.o app_module.o gxh_structure.o sqlite3.o		
	
app_module_gate.o: app_module_gate.cpp 	 
	$(XCC) $(CFLAGS) app_module_gate.cpp -o $@ 
	$(XCC) $(SHARED) app_module_gate.so $@ gxh_scope.o app_module.o gxh_structure.o sqlite3.o		


app_module_wsdl.o: app_module_wsdl.cpp 	 
	$(XCC) $(CFLAGS) app_module_wsdl.cpp -o $@ 
	$(XCC) $(SHARED) app_module_wsdl.so $@ gxh_scope.o app_module.o gxh_structure.o sqlite3.o	 
	
app_module_sensor.o: app_module_sensor.cpp 	 
	$(XCC) $(CFLAGS) app_module_sensor.cpp -o $@ 
	$(XCC) $(SHARED) app_module_sensor.so $@ gxh_scope.o app_module.o gxh_structure.o sqlite3.o		
	
app_module_provent.o: app_module_provent.cpp 	 
	$(XCC) $(CFLAGS) app_module_provent.cpp -o $@ 
	$(XCC) $(SHARED) app_module_provent.so $@ gxh_scope.o app_module.o gxh_structure.o sqlite3.o 	
	
app_module_scene.o: app_module_scene.cpp 	 
	$(XCC) $(CFLAGS) app_module_scene.cpp -o $@ 
	$(XCC) $(SHARED) app_module_scene.so $@ gxh_scope.o app_module.o gxh_structure.o sqlite3.o 		
	
app_module_system.o: app_module_system.cpp 	 
	$(XCC) $(CFLAGS) app_module_system.cpp -o $@ 
	$(XCC) $(SHARED) app_module_system.so $@ gxh_scope.o app_module.o gxh_structure.o sqlite3.o 	
	
app_module_ethernet.o: app_module_ethernet.cpp 	 
	$(XCC) $(CFLAGS) app_module_ethernet.cpp -o $@
	$(XCC) $(SHARED) app_module_ethernet.so $@ gxh_scope.o app_module.o gxh_structure.o sqlite3.o 	
	
	
app_module_outs.o: app_module_outs.cpp 	 
	$(XCC) $(CFLAGS) app_module_outs.cpp -o $@
	$(XCC) $(SHARED) app_module_outs.so $@ gxh_scope.o app_module.o gxh_structure.o sqlite3.o 		


app_module_timer.o: app_module_timer.cpp 	 
	$(XCC) $(CFLAGS) app_module_timer.cpp -o $@
	$(XCC) $(SHARED) app_module_timer.so $@ gxh_scope.o app_module.o gxh_structure.o sqlite3.o h_sunset.o


app_module_camera.o: app_module_camera.cpp 	 
	$(XCC) $(CFLAGS) app_module_camera.cpp -o $@
	$(XCC) $(SHARED) app_module_camera.so $@ gxh_scope.o app_module.o gxh_structure.o sqlite3.o 
	
app_module_satel.o: app_module_satel.cpp 	 
	$(XCC) $(CFLAGS) app_module_satel.cpp -o $@
	$(XCC) $(SHARED) app_module_satel.so $@ gxh_scope.o app_module.o gxh_structure.o sqlite3.o 	
	
app_module_door.o: app_module_door.cpp 	 
	$(XCC) $(CFLAGS) app_module_door.cpp -o $@
	$(XCC) $(SHARED) app_module_door.so $@ gxh_scope.o app_module.o gxh_structure.o sqlite3.o  	
	
	
app_module_netronix.o: app_module_netronix.cpp 	 
	$(XCC) $(CFLAGS) app_module_netronix.cpp -o $@
	$(XCC) $(SHARED) app_module_netronix.so $@ gxh_scope.o app_module.o gxh_structure.o sqlite3.o  	

app_module_alarm.o: app_module_alarm.cpp 	 
	$(XCC) $(CFLAGS) app_module_alarm.cpp -o $@
	$(XCC) $(SHARED) app_module_alarm.so $@ gxh_scope.o app_module.o gxh_structure.o sqlite3.o  	
	
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------

gxh: sqlite3.o gxh_structure.o main.o gxh_scope.o gxh_core.o app_modbus_rtu.o app_driver.o app_port.o app_driver_techbase.o app_driver_elpiast.o app_driver_netronix.o app_driver_provent.o app_driver_satel.o app_port_ethernet.o app_port_serial.o app_zi_queue.o app_zc_queue.o app_module.o h_sunset.o app_module_wsdl.o app_module_sensor.o app_module_gate.o app_module_blind.o app_module_light.o app_module_techbase.o app_module_elpiast.o app_module_provent.o app_module_scene.o app_module_system.o app_module_ethernet.o app_module_outs.o app_module_camera.o app_module_satel.o  app_module_door.o app_module_netronix.o app_module_alarm.o app_module_timer.o
	@echo "Link..................................." $@
	
	#dla cortexa..	npe x1000
	$(LD) $(LDFLAGS) $@ gxh_structure.o main.o gxh_scope.o gxh_core.o app_modbus_rtu.o app_driver.o app_port.o app_port_ethernet.o app_port_serial.o app_zi_queue.o app_zc_queue.o sqlite3.o h_sunset.o
	
	 
	@echo  "\033[0;31m"
	@echo  "Link................................ 100%" $@
	@echo  "\033[0m"	 	
 
	@echo  "......................................"
	@echo  ".                                    ."
	@echo  ".              GXH 5.0.11             ."
	@echo  ".                                     ." 
	@echo  ".         auth. Błażej Kita          ."
	@echo  ".                                    ."
	@echo  "......................................"
  

clean:
	-rm -f *.map *.o *.so gxh

 
.PHONY: all celan	
