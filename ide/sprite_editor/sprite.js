var canvas = null;
var context = null;

var spriteZoomFactor = 3.0;
var currentSpriteZoomFactor = 3.0;
var showZoomCounter = 0;
var leftMouseDown = false;
var leftMouseXBase = 0;
var leftMouseYBase = 0;
var rightMouseDown = false;
var rightMouseXBase = 0;
var rightMouseYBase = 0;

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
    canvas = document.getElementById('spriteEditor')
    context = canvas.getContext('2d');

    InitializeCanvas();
}

function InitializeCanvas() 
{
    canvas.height = window.innerHeight ;
    canvas.width = window.innerWidth * 0.5;

    SetCanvasEvents();
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

function SetCanvasEvents()
{
    canvas.addEventListener('wheel',function(event)
    {
        spriteZoomFactor = Math.max(1.0, Math.min(10.0, spriteZoomFactor + event.deltaY/600));

        RenderZoom();

        event.preventDefault();//prevent zoom
        return false; 
    }, false);

    canvas.addEventListener('mousedown',function(event)
    {
        var width = 100 * currentSpriteZoomFactor;
        var height = 100 * currentSpriteZoomFactor;
        var baseX = canvas.width/2 - width/2 - currentSpriteOffsetX;
        var baseY = canvas.height/2 - height/2 - currentSpriteOffsetY;

        if(event.button === 1 || event.button === 0)
        {
            if (event.pageX >= baseX && event.pageX <= baseX + width &&
                event.pageY >= baseY && event.pageY <= baseY + height)
            {
                rightMouseDown = true;
                rightMouseXBase = event.pageX;
                rightMouseYBase = event.pageY;
                
                SetPixel(parseInt((event.pageX - baseX)/(width/8)), parseInt((event.pageY - baseY)/(width/8)));
            }
        }
        else if(event.button === 2)
        {
            leftMouseDown = true;
            leftMouseXBase = event.pageX + spriteOffsetX;
            leftMouseYBase = event.pageY + spriteOffsetY;
        }

        event.preventDefault();
        return false; 
    }, false);

    canvas.addEventListener('mouseup', function (event)
    {
        if(event.button === 2)
            leftMouseDown = false;
        else (event.button === 1 || event.button === 0)
            rightMouseDown = false;

        event.preventDefault();
        return false; 
    }, false);

    canvas.addEventListener('mousemove', function (event)
    {
        if (leftMouseDown)
        {
            spriteOffsetX = leftMouseXBase - event.pageX;
            spriteOffsetY = leftMouseYBase - event.pageY;
        }
        else if (rightMouseDown)
        {
            var width = 100 * currentSpriteZoomFactor;
            var height = 100 * currentSpriteZoomFactor;
            var baseX = canvas.width/2 - width/2 - currentSpriteOffsetX;
            var baseY = canvas.height/2 - height/2 - currentSpriteOffsetY;

            SetPixel(parseInt((event.pageX - baseX)/(width/8)), parseInt((event.pageY - baseY)/(width/8)));
        }

        event.preventDefault();
        return false; 
    }, false)

    canvas.addEventListener('contextmenu', function (event)
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

function RefreshCanvas()
{
    ClearCanvas();
    RenderSpriteBase(SpriteMode.Sprite8x8)
}

function ClearCanvas()
{
    context.clearRect(0, 0, canvas.width, canvas.height);
    context.fillStyle = 'rgba(45, 45, 45,1)';
    context.fillRect(0, 0, canvas.width, canvas.height);
}

function RenderSpriteBase(Mode)
{
    context.fillStyle = 'rgba(180,180,180,0.3)';
    
    currentSpriteZoomFactor += (spriteZoomFactor - currentSpriteZoomFactor)/5;
    
    currentSpriteOffsetX += (spriteOffsetX - currentSpriteOffsetX)/5;
    currentSpriteOffsetY += (spriteOffsetY - currentSpriteOffsetY)/5;

    var width = 100 * currentSpriteZoomFactor;
    var height = 100 * currentSpriteZoomFactor;
    var baseX = canvas.width/2 - width/2 - currentSpriteOffsetX;
    var baseY = canvas.height/2 - height/2 - currentSpriteOffsetY;

    context.fillRect(baseX, baseY, width, height )
    
    // Render Pixels
    for (var i = 0; i < 8; i++)
    {
        for (var j = 0; j < 8; j++)
        {
            if (pixels[j][i] == 0)
                context.fillStyle = 'rgba(0, 0, 255, 1)';
            else
                context.fillStyle = 'rgba(255, 0, 0, 1)';

            context.fillRect(baseX + (width/8)*j, baseY + (height/8)*i, width/8, height/8 )
        }
    }

    // Sprite 8x8
    for (var i = 0; i < 9; i++)
    {
        context.strokeStyle = 'rgba(255,255,255,1)';

        context.beginPath();
        context.moveTo(baseX + (width/8)*i, baseY);
        context.lineTo(baseX + (width/8)*i, baseY + height);
        context.stroke();

        context.beginPath();
        context.moveTo(baseX, baseY + (height/8)*i);
        context.lineTo(baseX + width, baseY + (height/8)*i);
        context.stroke();
    }
}

function ReportWindowSize() 
{
    canvas.height = window.innerHeight ;
    canvas.width = window.innerWidth * 0.5;
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

    leftMouseXBase = 0;
    leftMouseYBase = 0;
    spriteOffsetX = 0;
    spriteOffsetY = 0;
}

function RenderZoom()
{
    showZoomCounter = 100;
}

function ShowZoomValue() 
{
    context.font = "20px sans serif";

    if (showZoomCounter <= 20)
        context.fillStyle = 'rgba(255,255,255,' + showZoomCounter/20 + ')';
    else
        context.fillStyle = 'rgba(255,255,255,1)';

    context.textAlign = "right";
    context.fillText(parseInt((parseInt((currentSpriteZoomFactor*10))/10)*100) + "%", canvas.width - 40, canvas.height - 30);
}

function SpriteRenderer() 
{
    RefreshCanvas();

    if (showZoomCounter != 0)
    {
        ShowZoomValue();
        showZoomCounter--;
    }
}

window.onkeydown = HandleKeyInput;
window.onresize = ReportWindowSize;