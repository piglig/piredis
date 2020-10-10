class MyClass {
    public onopen: Function = null;
}


let myClass = new MyClass();
myClass.onopen = function() {
    console.log("onopen");
};


myClass.onopen();

let date = Date.now();
console.log(date);