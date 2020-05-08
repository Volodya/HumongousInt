#include <iostream>
#include <sstream>
#include <string>

using LargeInt = unsigned long long;
#include "HumongousInt.hpp"

int main()
{
	std::cout << "Starting test" << std::endl;
	std::cout << std::hex << std::showbase;

	LargeInt large(0);
	HumongousInt huge(0);

	std::cout << "Large:     " << large << std::endl;
	std::cout << "Humongous: " << huge  << std::endl;

	++large;
	++huge;

	std::cout << "Large:     " << large << std::endl;
	std::cout << "Humongous: " << huge  << std::endl;

	large <<= 2;
	huge  <<= 2;

	std::cout << "Large:     " << large << std::endl;
	std::cout << "Humongous: " << huge  << std::endl;

	large >>= 2;
	huge  >>= 2;

	std::cout << "Large:     " << large << std::endl;
	std::cout << "Humongous: " << huge  << std::endl;

	large += 2;
	huge  += 2;

	std::cout << "Large:     " << large << std::endl;
	std::cout << "Humongous: " << huge  << std::endl;

	large &= (unsigned char)(15);
	huge  &= static_cast<unsigned char>(15);

	std::cout << "Large:     " << large << std::endl;
	std::cout << "Humongous: " << huge  << std::endl;

	large &= (unsigned long long)(15);
	huge  &= (unsigned long long)(15);

	std::cout << "Large:     " << large << std::endl;
	std::cout << "Humongous: " << huge  << std::endl;

	large |= (unsigned char)(17);
	huge  |= static_cast<unsigned char>(17);

	std::cout << "Large:     " << large << std::endl;
	std::cout << "Humongous: " << huge  << std::endl;

	large |= (unsigned long long)(37);
	huge  |= (unsigned long long)(37);

	std::cout << "Large:     " << large << std::endl;
	std::cout << "Humongous: " << huge  << std::endl;

	large ^= (unsigned long long)(42);
	huge  ^= (unsigned long long)(42);

	std::cout << "Large:     " << large << std::endl;
	std::cout << "Humongous: " << huge  << std::endl;

	large *= 10;
	huge  *= 10;

	std::cout << "Large:     " << large << std::endl;
	std::cout << "Humongous: " << huge  << std::endl;

	std::stringstream s1("0x123"), s2("0x123");
	s1 >> large;
	s2 >> huge;

	std::cout << "Large:     " << large << std::endl;
	std::cout << "Humongous: " << huge  << std::endl;

	std::string str = "0x135";
	large = std::stoull(str, 0, 16);
	huge = stohi(str);

	std::cout << "Large:     " << large << std::endl;
	std::cout << "Humongous: " << huge  << std::endl;
	
	large = 13900000000.;
	huge = HumongousInt(13900000000.);
	
	std::cout << "Large:     " << large << std::endl;
	std::cout << "Humongous: " << huge  << std::endl;

	large++;
	huge++;
	
	std::cout << "Large:     " << large << std::endl;
	std::cout << "Humongous: " << huge  << std::endl;
	
	large--;
	huge--;
	
	std::cout << "Large:     " << large << std::endl;
	std::cout << "Humongous: " << huge  << std::endl;
	
	++large;
	++huge;
	
	std::cout << "Large:     " << large << std::endl;
	std::cout << "Humongous: " << huge  << std::endl;
	
	--large;
	--huge;
	
	std::cout << "Large:     " << large << std::endl;
	std::cout << "Humongous: " << huge  << std::endl;
	
	large *= 5;
	huge *= 5;
	
	std::cout << "Large:     " << large << std::endl;
	std::cout << "Humongous: " << huge  << std::endl;
	
	std::cout << "===================================="  << std::endl;
	
	int choice;
	unsigned long long input;
	for(;;)
	{
		std::cout
			<< "0.  Exit" << std::endl
			<< "1.  <<= Shift left" << std::endl
			<< "2.  >>= Shift right" << std::endl
			<< "3.  +=  Add" << std::endl
			<< "4.  -=  Subtract" << std::endl
			<< "5.  *=  Multiply" << std::endl
			<< "6.  /=  Divide" << std::endl
			//<< "7.  %=  Modulo" << std::endl
			<< "8.  &=  Bit-AND" << std::endl
			<< "9.  |=  Bit-OR" << std::endl
			<< "10. ^=  Bit-XOR" << std::endl
		    << "Your choice: ";
		std::cin >> choice;
		std::cout << "Value (unsigned long long): ";
		std::cin >> input;

		switch(choice)
		{
			case 0:
				return 0;
				// break;
			case 1:
				large <<= input;
				huge <<= input;
				break;
			case 2:
				large >>= input;
				huge >>= input;
				break;
			case 3:
				large += input;
				huge += input;
				break;
			case 4:
				large -= input;
				huge -= HumongousInt(input);
				break;
			case 5:
				large *= input;
				huge *= input;
				break;
			case 6:
				large /= input;
				huge /= (HumongousInt)(input);
				break;
			case 7:
				//large %= input;
				//huge %= input;
				break;
			case 8:
				large &= input;
				huge &= input;
				break;
			case 9:
				large |= input;
				huge |= input;
				break;
			case 10:
				large ^= input;
				huge ^= input;
				break;
		}

		std::cout << "Large:     " << large << std::endl;
		std::cout << "Humongous: " << huge  << std::endl;
		
	}

	return 0;
}
