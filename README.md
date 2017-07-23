# Protocol Details

## Types 
* Discovery_Broadcast,
* Discovery Unicast,
* Response_Unicast,
* Chat,
* Chat_ack,
* Exit

## Examples

###### Discovery Query Broadcast [1] // who is there?

[Type][Requester_Name][Requester_Surname]		

> [1][Kivanc][Cakmak]

###### Discovery Query Unicast [2] // Is Devin here?

[Type][Request_Name][Requester_Surname][Target_Name][Target_Surname]

> [2][Kivanc][Cakmak][Devin][Mungan]

###### Hello Response [3] // answer from Devin

[Type][Responder Name][Responder Surname][Queryier Name][Queryier Surname]

> [3][Devin][Mungan][Kivanc][Cakmak]

###### Chat[4] // message to Devin

[Type][Length][packet_id][message] //length: 2 byte, packet_id: 1 byte

> [4][Length][1][bugun Pazartesi]

###### Chat Ack[5] // ack from Devin

[Type][packet_id]

> [5][1]

###### Exiting[6]

[Type][name][surname]

> [6][Kivanc][Cakmak]
