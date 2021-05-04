var editorCanvas = null;
var editorContext = null;

var spriteZoomFactor = 3.0;
var currentSpriteZoomFactor = 3.0;
var showZoomCounter = 0;
var rightMouseButtonDown = false;
var rightMouseButtonXBase = 0;
var rightMouseButtonYBase = 0;
var leftMouseButtonDown = false;
var leftMouseButtonXBase = 0;
var leftMouseButtonYBase = 0;
var holdingPaletteBox = false;

var paletteBoxX = 20;
var paletteBoxY = 20;
var paletteBoxWidth = 100;
var paletteBoxTitleHeight = 20;
var paletteBoxHeight = 200;
var paletteHoldBaseX = 0;
var paletteHoldBaseY = 0;

var selectedPalette = 1;

var spriteOffsetX = 0;
var spriteOffsetY = 0;
var currentSpriteOffsetX = 0;
var currentSpriteOffsetY = 0;
var pixels;
var spriteRendererTimer = setInterval(SpriteRenderer, 10);

const SpriteMode = 
{
    Sprite8x8 : 0,
    Sprite8x16 : 1,
}

function Initialize()
{
    editorCanvas = document.getElementById('spriteEditor')
    editorContext = editorCanvas.getContext('2d');
    
    InitializeCanvas();
}

function InitializeCanvas() 
{
    editorCanvas.height = document.getElementById('editorWindow').clientHeight;
    editorCanvas.width = document.getElementById('editorWindow').clientWidth;

    SetEditorCanvasEvents();
    ClearCanvas();
    InitializeLogicSprite();
}

function InitializeLogicSprite()
{
    pixels = new Array(8);

    for (var i = 0; i < 8; i++)
    {
        pixels[i] = new Array(8);
        for (var j = 0; j < 8; j++)
            pixels[i][j] = 0;
    }
}

function SetEditorCanvasEvents()
{
    editorCanvas.addEventListener('wheel',function(event)
    {
        spriteZoomFactor = Math.max(1.0, Math.min(10.0, spriteZoomFactor + event.deltaY/600));

        RenderZoom();

        event.preventDefault();//prevent zoom
        return false; 
    }, false);

    editorCanvas.addEventListener('mouseout',function(event)
    {
        rightMouseButtonDown = false;
        leftMouseButtonDown = false;
        holdingPaletteBox = false;
    });

    editorCanvas.addEventListener('mousedown',function(event)
    {
        var width = 100 * currentSpriteZoomFactor;
        var height = 100 * currentSpriteZoomFactor;
        var baseX = editorCanvas.width/2 - width/2 - currentSpriteOffsetX;
        var baseY = editorCanvas.height/2 - height/2 - currentSpriteOffsetY;

        if(event.button === 1 || event.button === 0)
        {
            if (event.pageX >= baseX && event.pageX <= baseX + width &&
                event.pageY >= baseY && event.pageY <= baseY + height)
            {
                leftMouseButtonDown = true;
                leftMouseButtonXBase = event.pageX;
                leftMouseButtonYBase = event.pageY;
                
                SetPixel(parseInt((event.pageX - baseX)/(width/8)), parseInt((event.pageY - baseY)/(width/8)));
            }
            else if (event.pageX >= paletteBoxX && event.pageX <= paletteBoxX + paletteBoxWidth &&
                     event.pageY >= paletteBoxY && event.pageY <= paletteBoxY + paletteBoxTitleHeight)
            {
                holdingPaletteBox = true;
                leftMouseButtonDown = true;
                leftMouseButtonXBase = event.pageX;
                leftMouseButtonYBase = event.pageY;
                paletteHoldBaseX = paletteBoxX - event.pageX;
                paletteHoldBaseY = paletteBoxY - event.pageY;
            }
            else if (event.pageX >= paletteBoxX && event.pageX <= paletteBoxX + paletteBoxWidth &&
                    event.pageY >= paletteBoxY + paletteBoxTitleHeight && event.pageY <= paletteBoxY + paletteBoxTitleHeight+ paletteBoxHeight)
            {
                if (event.pageY >= paletteBoxY + paletteBoxTitleHeight && event.pageY <= paletteBoxY + paletteBoxTitleHeight + paletteBoxHeight/4)
                    selectedPalette = 0;
                else if (event.pageY >= paletteBoxY + paletteBoxTitleHeight + (paletteBoxHeight/4) && event.pageY <= paletteBoxY + paletteBoxTitleHeight + 2*(paletteBoxHeight/4))
                    selectedPalette = 1;
                else if (event.pageY >= paletteBoxY + paletteBoxTitleHeight + 2*(paletteBoxHeight/4) && event.pageY <= paletteBoxY + paletteBoxTitleHeight + 3*(paletteBoxHeight/4))
                    selectedPalette = 2;
                else if (event.pageY >= paletteBoxY + paletteBoxTitleHeight + 3*(paletteBoxHeight/4) && event.pageY <= paletteBoxY + paletteBoxTitleHeight + 4*(paletteBoxHeight/4))
                    selectedPalette = 3;
            }
        }
        else if(event.button === 2)
        {
            rightMouseButtonDown = true;
            rightMouseButtonXBase = event.pageX + spriteOffsetX;
            rightMouseButtonYBase = event.pageY + spriteOffsetY;
        }

        event.preventDefault();
        return false; 
    }, false);

    editorCanvas.addEventListener('mouseup', function (event)
    {
        if(event.button === 2)
        {
            rightMouseButtonDown = false;
        }
        else (event.button === 1 || event.button === 0)
        {
            leftMouseButtonDown = false;
            holdingPaletteBox = false;
        }

        event.preventDefault();
        return false; 
    }, false);

    editorCanvas.addEventListener('mousemove', function (event)
    {
        if (rightMouseButtonDown)
        {
            spriteOffsetX = rightMouseButtonXBase - event.pageX;
            spriteOffsetY = rightMouseButtonYBase - event.pageY;
        }
        else if (leftMouseButtonDown)
        {
            if (!holdingPaletteBox)
            {
                var width = 100 * currentSpriteZoomFactor;
                var height = 100 * currentSpriteZoomFactor;
                var baseX = editorCanvas.width/2 - width/2 - currentSpriteOffsetX;
                var baseY = editorCanvas.height/2 - height/2 - currentSpriteOffsetY;
                SetPixel(parseInt((event.pageX - baseX)/(width/8)), parseInt((event.pageY - baseY)/(width/8)));
            }
            else
            {
                paletteBoxX = event.pageX + paletteHoldBaseX; //(leftMouseButtonXBase - event.pageX) + paletteHoldBaseX;
                paletteBoxY = event.pageY + paletteHoldBaseY;//(leftMouseButtonYBase - event.pageY) + paletteHoldBaseY;
            }

        }

        event.preventDefault();
        return false; 
    }, false)

    editorCanvas.addEventListener('contextmenu', function (event)
    {
        event.preventDefault();
        return false; 
    }, false)
}

function SetPixel(x, y)
{
    if (x >= 8 || x < 0 || y >= 8 || y < 0)
        return;

    var indexX = x;
    var indexY =  y;

    pixels[indexX][indexY] = 1;
    UpdateBytes();
}

function ToPaddedHexString(num, len) {
    str = num.toString(16).toUpperCase();
    return "0".repeat(len - str.length) + str;
}

function UpdateBytes()
{
    var line = "";

    for (var i = 0; i < 8; i++)
    {
        var row = 0
        for (var j = 0; j < 8; j++)
        {
            var color = pixels[j][i];
            row |= ((color & 0x02) >> 1)<<(7 - j)*2;
            row |= ((color & 0x01))<<(7 - j);
        }
        line += "0x" +  ToPaddedHexString(row, 4) + " ";
    }

    console.log(line);
}

function RenderCanvas()
{
    ClearCanvas();
    RenderSpriteBase(SpriteMode.Sprite8x8)
    RenderPalettes();
}

function RenderPalettes()
{
    editorContext.fillStyle = 'rgba(180, 180, 180,1)';
    editorContext.fillRect(paletteBoxX, paletteBoxY, paletteBoxWidth, paletteBoxTitleHeight);

    editorContext.font = "16px sans serif";
    editorContext.fillStyle = 'rgba(20,20,20,1)';
    editorContext.textAlign = "left";
    
    editorContext.fillText("Palettes", paletteBoxX, paletteBoxY + 16, paletteBoxWidth);

    // Rener Palette Colors
    editorContext.fillStyle = GetPaletteColor(0);
    editorContext.fillRect(paletteBoxX, paletteBoxY + paletteBoxTitleHeight, paletteBoxWidth, paletteBoxHeight/4);

    editorContext.fillStyle = GetPaletteColor(1);
    editorContext.fillRect(paletteBoxX, paletteBoxY + paletteBoxTitleHeight + paletteBoxHeight/4, paletteBoxWidth, paletteBoxHeight/4);

    editorContext.fillStyle = GetPaletteColor(2);
    editorContext.fillRect(paletteBoxX, paletteBoxY + paletteBoxTitleHeight + 2*paletteBoxHeight/4, paletteBoxWidth, paletteBoxHeight/4);

    editorContext.fillStyle = GetPaletteColor(3);
    editorContext.fillRect(paletteBoxX, paletteBoxY + paletteBoxTitleHeight + 3*paletteBoxHeight/4, paletteBoxWidth, paletteBoxHeight/4);

    editorContext.fillStyle = 'rgba(90, 90, 90, 0.8)';
    editorContext.fillRect(paletteBoxX, paletteBoxY + paletteBoxTitleHeight + (paletteBoxHeight/4)*selectedPalette, paletteBoxWidth, paletteBoxHeight/4);
    editorContext.fillStyle = GetPaletteColor(selectedPalette);
    editorContext.fillRect(paletteBoxX + 10, paletteBoxY + (paletteBoxHeight/4)*selectedPalette + paletteBoxTitleHeight + 10, paletteBoxWidth - 20, paletteBoxHeight/4 - 20);
}

function GetPaletteColor(paletteIndex)
{
    if (paletteIndex == 0)
        return 'rgba(255, 0, 0,1)';
    else if (paletteIndex == 1)
        return 'rgba(0, 255, 0,1)';
    else if (paletteIndex == 2)
        return 'rgba(0, 0, 255,1)';
    else if (paletteIndex == 3)
        return 'rgba(255, 0, 255,1)';;
}

function ClearCanvas()
{
    editorContext.clearRect(0, 0, editorCanvas.width, editorCanvas.height);
    editorContext.fillStyle = 'rgba(45, 45, 45,1)';
    editorContext.fillRect(0, 0, editorCanvas.width, editorCanvas.height);
}

function RenderSpriteBase(Mode)
{
    editorContext.fillStyle = 'rgba(180,180,180,0.3)';
    
    currentSpriteZoomFactor += (spriteZoomFactor - currentSpriteZoomFactor)/5;
    
    currentSpriteOffsetX += (spriteOffsetX - currentSpriteOffsetX)/5;
    currentSpriteOffsetY += (spriteOffsetY - currentSpriteOffsetY)/5;

    var width = 100 * currentSpriteZoomFactor;
    var height = 100 * currentSpriteZoomFactor;
    var baseX = editorCanvas.width/2 - width/2 - currentSpriteOffsetX;
    var baseY = editorCanvas.height/2 - height/2 - currentSpriteOffsetY;

    editorContext.fillRect(baseX, baseY, width, height )
    
    // Render Pixels
    for (var i = 0; i < 8; i++)
    {
        for (var j = 0; j < 8; j++)
        {
            if (pixels[j][i] == 0)
                editorContext.fillStyle = 'rgba(0, 0, 255, 1)';
            else
                editorContext.fillStyle = 'rgba(255, 0, 0, 1)';

            editorContext.fillRect(baseX + (width/8)*j, baseY + (height/8)*i, width/8, height/8 )
        }
    }

    // Sprite 8x8
    for (var i = 0; i < 9; i++)
    {
        editorContext.strokeStyle = 'rgba(255,255,255,1)';

        editorContext.beginPath();
        editorContext.moveTo(baseX + (width/8)*i, baseY);
        editorContext.lineTo(baseX + (width/8)*i, baseY + height);
        editorContext.stroke();

        editorContext.beginPath();
        editorContext.moveTo(baseX, baseY + (height/8)*i);
        editorContext.lineTo(baseX + width, baseY + (height/8)*i);
        editorContext.stroke();
    }
}

function ReportWindowSize() 
{
    editorCanvas.height = document.getElementById('editorWindow').clientHeight;
    editorCanvas.width = document.getElementById('editorWindow').clientWidth;;
}

function HandleKeyInput(e) 
{
    // Ctrl + 0
    if (e.keyCode == '48' && (e.ctrlKey || e.metaKey)) 
    {
        ResetZoomAndPosition();
        RenderZoom();
    }
    else if (e.keyCode == '187' && (e.ctrlKey || e.metaKey)) 
    {
        spriteZoomFactor = Math.max(1.0, Math.min(10.0, spriteZoomFactor + 1));
        RenderZoom();
    }
    else if (e.keyCode == '189' && (e.ctrlKey || e.metaKey)) 
    {
        spriteZoomFactor = Math.max(1.0, Math.min(10.0, spriteZoomFactor - 1));
        RenderZoom();
    }

    e.preventDefault();
}

function ResetZoomAndPosition()
{
    if (spriteZoomFactor > 3)
        spriteZoomFactor = 3;
    else
        spriteZoomFactor = 3.1;

    rightMouseButtonXBase = 0;
    rightMouseButtonYBase = 0;
    spriteOffsetX = 0;
    spriteOffsetY = 0;
}

function RenderZoom()
{
    showZoomCounter = 100;
}

function ShowZoomValue() 
{
    editorContext.font = "20px sans serif";

    if (showZoomCounter <= 20)
        editorContext.fillStyle = 'rgba(255,255,255,' + showZoomCounter/20 + ')';
    else
        editorContext.fillStyle = 'rgba(255,255,255,1)';

    editorContext.textAlign = "right";
    editorContext.fillText(parseInt((parseInt((currentSpriteZoomFactor*10))/10)*100) + "%", editorCanvas.width - 40, editorCanvas.height - 30);
}

function SpriteRenderer() 
{
    RenderCanvas();

    if (showZoomCounter != 0)
    {
        ShowZoomValue();
        showZoomCounter--;
    }
}

window.onkeydown = HandleKeyInput;
window.onresize = ReportWindowSize;