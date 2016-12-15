setTimeout(loop,100);
function loop(){
	setTimeout(loop,100);
}
process.on("SIGINT",function(){
	console.log("exit");
	process.exit();
});
