var WebSocket = require('ws');
// class MyClass {
//     public onopen: Function = null;
//     MyClassName: string;
//     MyClassNumber: number;
// }
// let myClass = new MyClass();
// myClass.MyClassNumber = 100;
// myClass.onopen = function() {
//     console.log("onopen");
// };
// myClass.onopen();
// let date = Date.now();
// console.log(date);
// function f(_?) {
//     console.log(arguments);
// }
// f();
// let prefabPath =  "Game/prefabs/Game/GameMenu";
// console.log(prefabPath.split(",")[1]);
// console.log(myClass.hasOwnProperty('MyClassNumber'));
var ws = new WebSocket('ws://10.10.243.226:8000/');
ws.onmessage = function (event) {
    console.log(event.data);
};
