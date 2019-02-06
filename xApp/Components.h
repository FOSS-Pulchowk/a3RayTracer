// STL's !!!!
#include <vector>
#include "Utility/string2.h"

struct cpu {
	a3::String name;
	float speed;
	short unsigned int core;
	unsigned int power;
	float avgRate;
	float price;
};

struct motherboard {
	a3::String name;
	a3::String socket;
	a3::String formFactor;
	short unsigned int ramSlot;
	short unsigned int maxRam;
	float avgRate;
	float price;
};

struct cpuCooler{
	a3::String name;
	unsigned int minRpm;
	unsigned int maxRpm;
	unsigned int noiseLevel;
	float avgRate;
	float price;
};

struct memory {
	a3::String name, speed, type;
	int cas;
	unsigned int numModule, eachModule, size;
	float avgRating;
	float Price;
};

struct storage {
	a3::String name, series;
	a3::String form, type, capacity;
	unsigned int cache;
	float avgRate;
	float price;
};

struct videoCard {
	a3::String name, series, chipset;
	unsigned int memory;
	float ClkCore;
	float avgRate;
	float Price;
};

struct powerSupply {
	a3::String name, series, form, efficiency;
	int power;
	a3::String modular;
	float avgRate;
	float price;
};

struct casing{
	a3::String name, type;
	float price;
};