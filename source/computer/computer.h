//
// Created by sixgales on 01.07.19.
//

#ifndef COMPUTER_COMPUTER_H
#define COMPUTER_COMPUTER_H

struct Particle
{

};

class Computer
{
public:
	virtual void init() = 0;
	virtual Particle *iterate() = 0;
};

Computer *getInstanceOf();

#endif //COMPUTER_COMPUTER_H
