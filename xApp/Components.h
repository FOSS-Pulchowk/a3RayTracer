// STL's !!!!
#include <vector>
#include "Utility/DString.h"

struct cpu {
	a3::dstring name;
	float speed;
	short unsigned int core;
	unsigned int power;
	float avgRate;
	float price;
};

struct motherboard {
	a3::dstring name;
	a3::dstring socket;
	a3::dstring formFactor;
	short unsigned int ramSlot;
	short unsigned int maxRam;
	float avgRate;
	float price;
};

struct cpuCooler{
	a3::dstring name;
	unsigned int minRpm;
	unsigned int maxRpm;
	unsigned int noiseLevel;
	float avgRate;
	float price;
};

struct memory {
	a3::dstring name, speed, type;
	int cas;
	unsigned int numModule, eachModule, size;
	float avgRating;
	float Price;
};

struct storage {
	a3::dstring name, series;
	a3::dstring form, type, capacity;
	unsigned int cache;
	float avgRate;
	float price;
};

struct videoCard {
	a3::dstring name, series, chipset;
	unsigned int memory;
	float ClkCore;
	float avgRate;
	float Price;
};

struct powerSupply {
	a3::dstring name, series, form, efficiency;
	int power;
	a3::dstring modular;
	float avgRate;
	float price;
};

struct casing{
	a3::dstring name, type;
	float price;
};