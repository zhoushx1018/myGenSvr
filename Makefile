
INSTALL_DIR=$(HOME)/run/myGenSvr
INSTALL_BIN_DIR = $(INSTALL_DIR)/bin

all:srv

srv:
	@cd ./BaseCommon; make
	@cd ./BaseServer; make
	@cd ./src; make

clean:
	@cd ./BaseCommon; make clean
	@cd ./BaseServer; make clean
	@cd ./src; make clean

install:
	make srv
	@cd ./src; make install
	
	dos2unix $(PWD)/Shell/ka $(PWD)/Shell/psall $(PWD)/Shell/sa
	dos2unix $(PWD)/src/*.ini
	
	ln -s $(PWD)/src/myGenSvr.ini $(INSTALL_BIN_DIR)/myGenSvr.ini 
	ln -s $(PWD)/Shell/ka $(INSTALL_BIN_DIR)/ka 
	ln -s $(PWD)/Shell/sa $(INSTALL_BIN_DIR)/sa 
	ln -s $(PWD)/Shell/psall $(INSTALL_BIN_DIR)/psall 
	
	chmod 744 $(INSTALL_BIN_DIR)/ka $(INSTALL_BIN_DIR)/sa $(INSTALL_BIN_DIR)/psall
	
	
installbin:
	make srv
	@cd ./src; make installbin

dist:
	make clean
	tar -zcvf ~/bak/myGenSvr.`date +%Y%m%d`.tar.gz ./
	du -sh
