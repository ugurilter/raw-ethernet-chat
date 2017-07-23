# Protocol Details

## Types 
* Discovery_Broadcast,
* Discovery Unicast,
* Response_Unicast,
* Chat,
* Chat_ack,
* Exit

## Examples

|#|Type|Definition|Example|Description|
|-|----|----------|-------|-----------|
|1|DQB|[type][name][sname]|[1][ugur][ilter]|Who is there ?|
|2|DQU|[type][name][sname][name][sname]|[2][ugur][ilter][john][doe]|Is 'X' here ?|
|3|HRESP|[type][name][sname][name][sname]|[3][john][doe][ugur][ilter]|Answer from 'X' ?|
|4|CHAT|[type][length][pid][msg]|[4][13][0][Hello World !]|MSG to 'X'|
|5|CHAT_ACK|[type][pid]|[5][0]|ACK from 'X'|
|6|EXIT|[type][name][sname]|[6][ugur][ilter]|I'm out, peace !|

## Usage

1. Open a terminal (CTRL + ALT + T),
2. Change working directory,
```
make
sudo ./chatApp
```
3. Start messaging.
