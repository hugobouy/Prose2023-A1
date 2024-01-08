# Makefile to generate custom JMeter class
# Created by Hugo BOUY
# Date: 09/06/2023

# Variables definitions
SOURCE_JMETER := /Users/hugobouy/Documents/ESEO/E4e_LOCAL/S8_TEST/FRAMEWORK/apache-jmeter-5.5
CUSTOM_CLASS_SOURCE := java/perso
#CLASS_NAME := LengthPrefixedBinaryTCPClientImpl
CLASS_NAME := LengthPrefixedBinaryTCPClientCustom

all:
	javac -cp $(SOURCE_JMETER)/lib/ext/ApacheJMeter_core.jar:$(SOURCE_JMETER)/lib/ext/ApacheJMeter_tcp.jar:$(SOURCE_JMETER)/lib/jorphan.jar:$(SOURCE_JMETER)/lib/slf4j-api-1.7.36.jar $(CUSTOM_CLASS_SOURCE)/$(CLASS_NAME).java

clean:
	rm -f $(CUSTOM_CLASS_SOURCE)/$(CLASS_NAME).class

