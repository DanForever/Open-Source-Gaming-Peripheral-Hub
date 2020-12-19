# Data format

Automatic profile appears to only write 6 lots of 16 bytes (96 bytes total)
Onboard profile appears to write 17 lots of 16 bytes (272 bytes total)

## Known Values

|Byte|Description|
|---|---|
|09| X axis DPI|
|10| Y axis DPI|
|33| [USB Polling rate](https://github.com/cvuchener/g500/blob/master/doc/commands/64-usbrate.md)|
|44-82|Button mappings|

## DPI
|Min (Decimal)|Max (Decimal)|Increment|Min (Hex)|Max (Hex)|
|-------------|-------------|---------|---------|---------|
|200          |8200         |50       |04       |A4       |

## Button Mappings
The G700S has 13 bindable buttons (Left, Middle, Right, G4-G11, mouse wheel left, mouse wheel right)
Scroll up/down does not appear to be rebindable (and neither is the button that releases the ratchet on the mouse wheel, as that's purely mechanical)

|Byte|Description|
|----|-----------|
|01  |Binding mode|
|02  |Mouse button|
|03  |Key binding|

(This is WIP and may be wrong - I suspect bytes 2 and 3 are more fluid than this)
|Mode|Descripion|
|----|----------|
|81  |Mapped to mouse button, use the next byte to determine what the mouse button should do|
|82  |Mapped to key strokes, examine the 3rd byte to determine functionality)

## Example captured automatic profile

|Byte          |00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
|Purpose       | ?| ?| ?| ?| ?| ?| ?| ?| ?|X Axis DPI|Y Axis DPI| ?| ?| ?| ?| ?|
|Example Value |01|00|00|00|00|00|4a|00|00|10|10|00|00|00|00|00|

|Byte          |16|17|18|19|20|21|22|23|24|25|26|27|28|29|30|31|
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
|Purpose       | ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?|
|Example Value |00|00|00|00|00|00|00|00|00|00|00|00|00|00|80|01|

|Byte          |32|33|34|35|36|37|38|39|40|41|42|43|44|45|46|47|
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
|Purpose       | ?|USB Polling rate| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?|
|Example Value |10|02|01|2c|02|58|64|ff|bc|00|09|31|81|01|00|81|

|Byte          |48|49|50|51|52|53|54|55|56|57|58|59|60|61|62|63|
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
|Purpose       | ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?|
|Example Value |02|00|81|04|00|81|08|00|81|10|00|81|20|00|81|40|

|Byte          |64|65|66|67|68|69|70|71|72|73|74|75|76|77|78|79|
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
|Purpose       | ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?|
|Example Value |00|83|03|00|81|00|01|81|00|02|81|00|04|81|00|08|

|Byte          |80|81|82|83|84|85|86|87|88|89|90|91|92|93|94|95|
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
|Purpose       | ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?| ?|
|Example Value |81|00|10|00|00|00|00|00|00|00|00|00|00|00|00|00|