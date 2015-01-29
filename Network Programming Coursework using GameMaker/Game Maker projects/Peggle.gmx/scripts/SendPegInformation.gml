buffer = argument[0];
peg_name = argument[1];
socket = argument[2];

buffer_seek(buffer, buffer_seek_start, 0); //start writing at beginning of buffer
buffer_write(buffer, buffer_u8, 2); //give message ID
buffer_write(buffer, buffer_string, peg_name); //send peg name
buffer_write(buffer, buffer_u32, asset_get_index(peg_name).x); //send peg x position
buffer_write(buffer, buffer_u32, asset_get_index(peg_name).y); //send peg y position
buffer_write(buffer, buffer_bool, asset_get_index(peg_name).isAlive); //is the peg active?
buffer_write(buffer, buffer_u32, current_time); //current server time
ReceivedPacket(buffer, socket);
