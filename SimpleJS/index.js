const images=document.getElementsByClassName("image");
let globalIndex=0;
let last={x:0,y:0};
function activate(image,x,y){
    image.style.transform=`translate(${x}px,${y}px)`
    image.dataset.status="active"
    image.style.zIndex=globalIndex;
    last={x,y};
}
window.onmousemove=e=>{
    if(Math.hypot(e.clientX-last.x,e.clientY-last.y)>50){
        const lead=images[globalIndex%images.length],
        tail=images[(globalIndex-6)%images.length];
        activate(lead,e.clientX,e.clientY);
        if(tail){
            tail.dataset.status="inactive";
        }
        globalIndex++;
    }
    if(globalIndex>images.length*10){
        globalIndex=6;
    }
}