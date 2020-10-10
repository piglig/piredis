var MyClass = /** @class */ (function () {
    function MyClass() {
        this.onopen = null;
    }
    return MyClass;
}());
var myClass = new MyClass();
myClass.onopen = function () {
    console.log("onopen");
};
myClass.onopen();
var date = Date.now();
console.log(date);
