#include <vector>
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
	//casing(std::string a, std::string b, float c ):name(a), type(b),price(c){}
};

std::vector<casing> vCase;
std::vector<powerSupply> vPowerSupply;
std::vector<videoCard> vVideoCard;
std::vector<storage> vStorage;
std::vector<memory> vMemory;
std::vector<cpuCooler> vCpuCooler;
std::vector<motherboard> vMotherboard;
std::vector<cpu> vCpu;

void setComponents() {
	vCase.push_back(casing{ "Thermaltake Core G3","ATX Mid Tower",69.49 });
	vCase.push_back(casing{ "Lian-Li PC-O10 WX","ATX Mid Tower",249.99 });

	vPowerSupply.push_back(powerSupply{"EVGA SuperNOVA 750","SuperNOVA G3","ATX","80+ Gold",750,"Full",4.5,99.89});
	vPowerSupply.push_back(powerSupply{"Corsair CX550M","CXM","ATX","80+ Bronze",550,"Semi",4.5,33.99});
	vPowerSupply.push_back(powerSupply{"Corsair TX650M Gold","TXM Gold","ATX","80+ Gold",650,"Semi",4.5,39.89});

	vVideoCard.push_back(videoCard{"MSI GeForce GTX 1060 6GT OCV1","GT OCV1","GeForce GTX 1060 6GB",6,1.544,4.5, 529.89});
	vVideoCard.push_back(videoCard{"Asus DUAL-GTX1070-O8G","Dual Series","GeForce GTX 1070",8,1.582,4.5, 439.89});
	vVideoCard.push_back(videoCard{"Gigabyte GV-RX570AORUS-4GD","AORUS 4G","Radeon RX 570",4,1.168,4, 139.99});

	vStorage.push_back(storage{"Western Digital WD10EZEX","Caviar Blue","3.5","7200 RPM","1 TB",64,4.5,44.89});
	vStorage.push_back(storage{"Samsung MZ-76E500B/AM","860 Evo","2.5","SSD","500 GB",512,4,82.99});
	vStorage.push_back(storage{"Intel SSDPECME040T401","DC P3608","PCI-E","SSD","4 TB",0,0,8867.99});

	vMemory.push_back(memory{ "Corsair Vengeance LPX","DDR4-3000","288-pin DIMM",15,2,8,16,4.5,114.99 });
	vMemory.push_back(memory{ "G.Skill Aegis","DDR4-3000","288-pin DIMM",15,2,8,16,4.5,94.99 });
	vMemory.push_back(memory{ "Team Vulcan","DDR4-3000","288-pin DIMM",15,2,4,8,4.5,60.88 });

	vCpuCooler.push_back(cpuCooler{ "Cooler Master Hyper 212 EVO",600,2000,36,4.5,27.89 });
	vCpuCooler.push_back(cpuCooler{"Cooler Master Hyper T2",0,2800,35,4,16.89});
	vCpuCooler.push_back(cpuCooler{ "be quiet! Dark Rock 4",0,1400,35,4.5,68.99 });

	vCpu.push_back(cpu{ "Intel Core i7 - 8700K",3.7,6,95,4.5,369.99});
	vCpu.push_back(cpu{ "Intel Core i9 - 9900K",3.6,8,95,4.5,529.99});
	vCpu.push_back(cpu{ "Intel Core i5 - 9600K",3.7,6,95,4.5,259.99});

	vMotherboard.push_back(motherboard{"MSI Z370-A PRO","LGA1151","ATX",4,64,4,109.89});
	vMotherboard.push_back(motherboard{"Asus PRIME Z390-A","LGA1151","ATX",4,64,0,183.00});
	vMotherboard.push_back(motherboard{"Gigabyte H310M A","LGA1151","Micro ATX",2,32,5,55.99});
	
};