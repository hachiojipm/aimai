build:
	platformio run

upload:
	platformio run --target upload

clean:
	platformio run --target clean

serial:
	pio serialports monitor

bootstrap:
	bash devtool/bootstrap.sh
