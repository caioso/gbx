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
var holdingToolBox = false;

var paletteName = "palette";
var paletteBoxX = 20;
var paletteBoxY = 450;
var paletteBoxWidth = 60;
var paletteBoxTitleHeight = 20;
var paletteBoxHeight = 200;
var paletteHoldBaseX = 0;
var paletteHoldBaseY = 0;

var toolBoxName = "toolbox";
var toolBoxX = 20;
var toolBoxY = 20;
var toolBoxWidth = 60;
var toolBoxTitleHeight = 20;
var toolBoxHeight = 400;
var toolBoxHoldBaseX = 0;
var toolBoxHoldBaseY = 0;

var selectedPalette = 0;

var spriteOffsetX = 0;
var spriteOffsetY = 0;
var currentSpriteOffsetX = 0;
var currentSpriteOffsetY = 0;
var pixels;
var spriteRendererTimer = setInterval(SpriteRenderer, 10);

var panelsList = new Array();

function Initialize()
{
    editorCanvas = document.getElementById('spriteEditor')
    editorContext = editorCanvas.getContext('2d');
    
    InitializeCanvas();
    InitializeZIndexs();
}

function InitializeZIndexs()
{
    panelsList.push(toolBoxName);
    panelsList.push(paletteName);
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
        holdingToolBox = false;
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
                SetPanelOnTop(paletteName);
                holdingPaletteBox = true;
                leftMouseButtonDown = true;
                leftMouseButtonXBase = event.pageX;
                leftMouseButtonYBase = event.pageY;
                paletteHoldBaseX = paletteBoxX - event.pageX;
                paletteHoldBaseY = paletteBoxY - event.pageY;
            }
            else if (event.pageX >= toolBoxX && event.pageX <= toolBoxX + toolBoxWidth &&
                     event.pageY >= toolBoxY && event.pageY <= toolBoxY + toolBoxTitleHeight)
            {
                SetPanelOnTop(toolBoxName);
                holdingToolBox = true;
                leftMouseButtonDown = true;
                leftMouseButtonXBase = event.pageX;
                leftMouseButtonYBase = event.pageY;
                toolBoxHoldBaseX = toolBoxX - event.pageX;
                toolBoxHoldBaseY = toolBoxY - event.pageY;
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
        else if (event.button === 1 || event.button === 0)
        {
            leftMouseButtonDown = false;
            holdingPaletteBox = false;
            holdingToolBox = false;
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
            if (holdingPaletteBox)
            {
                paletteBoxX = event.pageX + paletteHoldBaseX;
                paletteBoxY = event.pageY + paletteHoldBaseY;
            }
            else if (holdingToolBox)
            {
                toolBoxX = event.pageX + toolBoxHoldBaseX;
                toolBoxY = event.pageY + toolBoxHoldBaseY;
            }
            else
            {
                var width = 100 * currentSpriteZoomFactor;
                var height = 100 * currentSpriteZoomFactor;
                var baseX = editorCanvas.width/2 - width/2 - currentSpriteOffsetX;
                var baseY = editorCanvas.height/2 - height/2 - currentSpriteOffsetY;
                SetPixel(parseInt((event.pageX - baseX)/(width/8)), parseInt((event.pageY - baseY)/(width/8)));
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

function SetPanelOnTop(name)
{
    const index = panelsList.indexOf(name);
    if (index > -1)
        panelsList.splice(index, 1);

        panelsList.splice(0, 0, name);
}


function SetPixel(x, y)
{
    if (x >= 8 || x < 0 || y >= 8 || y < 0)
        return;

    var indexX = x;
    var indexY =  y;

    pixels[indexX][indexY] = selectedPalette;
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
        var rowMsb = 0
        var rowLsb = 0
        for (var j = 0; j < 8; j++)
        {
            var color = pixels[j][i];
            rowMsb |= ((color & 0x02) >> 1)<<(7 - j);
            rowLsb |= ((color & 0x01))<<(7 - j);
        }
        line += "" +  ToPaddedHexString((rowMsb), 2) + " ";
        line += "" +  ToPaddedHexString((rowLsb), 2) + " ";
    }

    console.log(line);
}

function RenderCanvas()
{
    ClearCanvas();
    RenderSpriteBase()
    RenderExtenders();

    for (var i = panelsList.length - 1; i >= 0; --i)
    {
        if (panelsList[i] == paletteName)
            RenderPalettes();
        else if (panelsList[i] == toolBoxName)
            RenderTools()
    }
}

function RenderExtenders()
{
    //currentSpriteOffsetX += (spriteOffsetX - currentSpriteOffsetX)/5;
    //currentSpriteOffsetY += (spriteOffsetY - currentSpriteOffsetY)/5;

    var width = 100 * currentSpriteZoomFactor;
    var height = 100 * currentSpriteZoomFactor;
    var baseX = editorCanvas.width/2 - width/2 - currentSpriteOffsetX;
    var baseY = editorCanvas.height/2 - height/2 - currentSpriteOffsetY;

    editorContext.fillStyle = 'rgba(90, 90, 90, 0.5)';
    editorContext.fillRect(baseX + width/2 - (width/8)/2, baseY - height*0.25, (width/8), (height/8));
    editorContext.fillRect(baseX + width/2 - (width/8)/2, baseY + height + (height/2)*0.25, (width/8), (height/8));
    editorContext.fillRect(baseX - (width/8) - (width/2)*0.25, baseY + height/2 - (height/8)/2, (width/8), (height/8));
    editorContext.fillRect(width + baseX + (width/2)*0.25, baseY + height/2 - (height/8)/2, (width/8), (height/8));
}

function RenderTools()
{
    
    editorContext.fillStyle = 'rgba(180, 180, 180,1)';
    editorContext.fillRect(toolBoxX, toolBoxY, toolBoxWidth, toolBoxTitleHeight);

    editorContext.font = "16px Arial, sans serif";
    editorContext.fillStyle = 'rgba(20,20,20,1)';
    editorContext.textAlign = "left";
    
    editorContext.fillText("Tools", toolBoxX, toolBoxY + 16, toolBoxWidth);

    // Render Background Colors
    editorContext.fillStyle = 'rgba(90,90,90,1)';
    editorContext.fillRect(toolBoxX, toolBoxY + toolBoxTitleHeight, toolBoxWidth, toolBoxHeight);
}

function RenderPalettes()
{
    editorContext.fillStyle = 'rgba(180, 180, 180,1)';
    editorContext.fillRect(paletteBoxX, paletteBoxY, paletteBoxWidth, paletteBoxTitleHeight);

    editorContext.font = "16px Arial, sans serif";
    editorContext.fillStyle = 'rgba(20,20,20,1)';
    editorContext.textAlign = "left";
    
    editorContext.fillText("Palettes", paletteBoxX, paletteBoxY + 16, paletteBoxWidth);

    // Render Palette Colors
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
    if (paletteIndex == 3)
        return 'rgba(8, 24, 32, 1)';
    else if (paletteIndex == 2)
        return 'rgba(52, 104, 86, 1)';
    else if (paletteIndex == 1)
        return 'rgba(136, 192, 112, 1)';
    else if (paletteIndex == 0)
        return 'rgba(224, 248, 208, 1)';
}

function ClearCanvas()
{
    editorContext.clearRect(0, 0, editorCanvas.width, editorCanvas.height);
    editorContext.fillStyle = 'rgba(45, 45, 45,1)';
    editorContext.fillRect(0, 0, editorCanvas.width, editorCanvas.height);
}

function RenderSpriteBase()
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
            editorContext.fillStyle = GetPaletteColor(pixels[j][i])
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
    editorContext.font = "20px Arial, sans serif";

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