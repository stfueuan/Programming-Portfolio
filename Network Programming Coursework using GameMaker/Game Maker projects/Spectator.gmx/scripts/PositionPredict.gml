currentAcc = argument[0];
currentVelX = argument[1];
currentVelY = argument[2];
currentPosX = argument[3];
currentPosY = argument[4];

timeInterval = 0.1; //we predict a tenth of a second ahead of where we are now

//standard position quadratic formula - s = at^2 + bt + c where a is acceleration, b is velocity and c is current position
pegDestroyer.nextPosX = currentAcc*power(timeInterval, 2) + currentVelX*timeInterval + currentPosX;
pegDestroyer.nextPosY = currentAcc*power(timeInterval, 2) + currentVelY*timeInterval + currentPosY;