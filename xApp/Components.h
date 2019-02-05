// STL's !!!!
#include <vector>
#include <string>

struct cpu {
	std::string name;
	float speed;
	short unsigned int core;
	unsigned int power;
	float avgRate;
	float price;
};

struct motherboard {
	std::string name;
	std::string socket;
	std::string formFactor;
	short unsigned int ramSlot;
	short unsigned int maxRam;
	float avgRate;
	float price;
};

struct cpuCooler{
	std::string name;
	unsigned int minRpm;
	unsigned int maxRpm;
	unsigned int noiseLevel;
	float avgRate;
	float price;
};

struct memory {
	std::string name, speed, type;
	int cas;
	unsigned int numModule, eachModule, size;
	float avgRating;
	float Price;
};

struct storage {
	std::string name, series;
	std::string form, type, capacity;
	unsigned int cache;
	float avgRate;
	float price;
};

struct videoCard {
	std::string name, series, chipset;
	unsigned int memory;
	float ClkCore;
	float avgRate;
	float Price;
};

struct powerSupply {
	std::string name, series, form, efficiency;
	int power;
	std::string modular;
	float avgRate;
	float price;
};

struct casing{
	std::string name, type;
	float price;
};