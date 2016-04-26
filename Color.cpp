#include "Color.h"
Color::Color()
{
    //set values for default constructor
    setType("null");
    setColour(Scalar(0,0,0));

}

Color::Color(string name){

    setType(name);

    if(name=="Purple"){

        //TODO: use "calibration mode" to find HSV min
        //and HSV max values

        setHSVmin(Scalar(125,54,62));//(0,187,0));
        setHSVmax(Scalar(163,149,123));//(6,256,256));

        //BGR value for Red:
        setColour(Scalar(0,0,255));

    }
    if(name=="Pink"){

        //TODO: use "calibration mode" to find HSV min
        //and HSV max values

        setHSVmin(Scalar(125,141,143));//(16,141,196));//(12,149,65));//(12,149,65));
        setHSVmax(Scalar(224,222,256));//(181,230,256));//(181,230,230)); //(181,222,256));

        //BGR value for Pink:
        setColour(Scalar(0,255,255));

    }
    if(name=="Blue"){

        //TODO: use "calibration mode" to find HSV min
        //and HSV max values

        setHSVmin(Scalar(58,97,83));
        setHSVmax(Scalar(125,256,216));

        //BGR value for Blue:
        setColour(Scalar(255,0,0));

    }

        if(name=="Green"){

        //TODO: use "calibration mode" to find HSV min
        //and HSV max values

        setHSVmin(Scalar(40,20,67));
        setHSVmax(Scalar(93,125,240));

        //BGR value for black to write the text beside the object:
        setColour(Scalar(0,255,0));

    }

    if(name=="Orange"){

        //TODO: use "calibration mode" to find HSV min
        //and HSV max values

        setHSVmin(Scalar(0,220,179));//(0,179,177));
        setHSVmax(Scalar(163,256,224));//(38,256,242));

        //BGR value for black to write the text beside the object:
        setColour(Scalar(0,255,0));

    }

}

Color::~Color(void)
{
}

int Color::getXPos(){

    return Color::xPos;

}

void Color::setXPos(int x){

    Color::xPos = x;

}

void Color::setCritical(bool critical){

    Color::critical = critical;
}

bool Color::getCritical(){

    return Color::critical;
}

void Color::setLane(int type){

    Color::Lane = type;
}

int Color::getLane(){

    return Color::Lane;
}

void Color::setSource(int SrcLane){

    Color::Src = SrcLane;
}


int Color::getSource(){

    return Color::Src;
}


void Color::setDestination(int DstLane){

    Color::Dst = DstLane;
}


int Color::getDestination(){

    return Color::Dst;
}

int Color::getYPos(){

    return Color::yPos;

}

void Color::setYPos(int y){

    Color::yPos = y;

}

Scalar Color::getHSVmin(){

    return Color::HSVmin;

}
Scalar Color::getHSVmax(){

    return Color::HSVmax;
}

void Color::setHSVmin(Scalar min){

    Color::HSVmin = min;
}


void Color::setHSVmax(Scalar max){

    Color::HSVmax = max;
}
