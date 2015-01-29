var buffer = argument[0]; //this is how gamemaker sets how many parameters are to be passed
buffer_seek(buffer, buffer_seek_start, 0);
var MSGid = buffer_read(buffer, buffer_u8); //read the message ID from the buffer

switch(MSGid){
case 1 : //synchronise clocks when game starts
    var time = buffer_read(buffer, buffer_u32); //get time client when message was sent
    var server_time = buffer_read(buffer, buffer_u32); //get current server time
    Client.latency = (current_time - time)/2; //calculate latency
    Client.delta = server_time - current_time + Client.latency; //calculate server delta
    break;
case 2 : //get new peg information
    peg_name = buffer_read(buffer, buffer_string);
    peg_x = buffer_read(buffer, buffer_u32);
    peg_y = buffer_read(buffer, buffer_u32);
    isAlive = buffer_read(buffer, buffer_bool);
    newServerTime = buffer_read(buffer, buffer_bool);
    instance_create(peg_x, peg_y, asset_get_index(peg_name));
    break;
case 3 : //get new ball information (so long as there are none already on screen)
    var balls = instance_number(pegDestroyer);

    if(balls < 1)
    {
        ball_name = buffer_read(buffer, buffer_string);
        ball_x = buffer_read(buffer, buffer_u32);
        ball_y = buffer_read(buffer, buffer_u32);
        
        with(instance_create(ball_x, ball_y, asset_get_index(ball_name))) //create instance of ball and use its local variables
        {
            ball_direction_x = buffer_read(buffer, buffer_u32);
            ball_speed = buffer_read(buffer, buffer_u32);
            newServerTime = buffer_read(buffer, buffer_bool);
            direction = ball_direction_x;
            initialSpeed = ball_speed;
            speed = ball_speed;
        }
    }
    break;
case 4 : //get current cannon information
    cannon_direction = buffer_read(buffer, buffer_u32);
    newServerTime = buffer_read(buffer, buffer_u32);
    dispenser.image_angle = lerp(dispenser.image_angle, cannon_direction, 1); //lerp a fifth of the distance per step
    break;
case 5 : //get peg collision information
    ball_x = buffer_read(buffer, buffer_u32);
    ball_y = buffer_read(buffer, buffer_u32);
    peg_name = buffer_read(buffer, buffer_string);
    newServerTime = buffer_read(buffer, buffer_u32); 

    if(instance_exists(15)) //check if pegDestroyer still exists in the game world
    {
        pegDestroyer.collisionOccuring = true;
    }
    break;
case 6 : //get current ball information
    with(pegDestroyer)
    {
        ball_x = buffer_read(buffer, buffer_u32);
        ball_y = buffer_read(buffer, buffer_u32);
        ball_speed = buffer_read(buffer, buffer_u32);
        newServerTime = buffer_read(buffer, buffer_u32);
        
        if(collisionCounter != 0)
        {
            //if guard allows for a precision of (+/-)100ms
            if((current_time + Client.delta) < (Client.newServerTime - 100) || (current_time + Client.delta) > (Client.newServerTime + 100))
            {
                speed = ball_speed;
                if(!collisionOccuring)
                {
                    acceleration = (speed - initialSpeed)/0.1; //where 0.1 is the time between predections
                    show_debug_message(string(acceleration));
                    PositionPredict(acceleration, hspeed, vspeed, ball_x, ball_y); //predict next position - a tenth of a second ahead of where we are now
                    move_towards_point(nextPosX, nextPosY, speed); //interpolation is done linearly using this function
                }
                initialSpeed = speed; //reset 'initialSpeed' to be your current speed for future acceleration calculations
            }
            else
            {
                //clearly the latency is too high to have interpolation take any effect, jump to newest known location
                x = ball_x;
                y = ball_x;
                speed = ball_speed;
                direction = ball_direction;
            }
        }
    }
    break;
}
