"use strict";
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
Object.defineProperty(exports, "__esModule", { value: true });
const omnicast_1 = require("./omnicast");
const native_ui_1 = require("./native-ui");
class OnSearch extends native_ui_1.TextChangedAction {
}
class OnRowChanged extends native_ui_1.CurrentRowChangedAction {
}
class TimeoutOkay extends native_ui_1.Action {
}
class FruitList extends native_ui_1.Component {
    constructor() {
        super(...arguments);
        this.items = [
            { name: 'banana' },
            { name: 'strawberry' },
            { name: 'blueberry' },
            { name: 'orange' },
            { name: 'abricot' },
            { name: 'pineapple' },
        ];
        this.activeItem = this.items[0];
        this.selected = 0;
    }
    onMount() {
        console.log(`mounted ${this.type}`);
    }
    update(action) {
        if (action instanceof OnRowChanged) {
            this.selected = action.value;
            console.log({ selected: this.selected });
        }
    }
    render() {
        console.log(`render list: ${this.selected}`);
        return (new native_ui_1.List(...this.items.map(({ name }) => new native_ui_1.ListItem(name)))
            .selected(this.selected)
            .currentRowChanged(OnRowChanged));
    }
}
;
class Application extends native_ui_1.Component {
    constructor() {
        super(...arguments);
        this.inputValue = "";
        this.count = 0;
    }
    onMount() {
        console.log('mounted');
    }
    update(action) {
        if (action instanceof OnSearch) {
            this.inputValue = action.value;
        }
        ++this.count;
    }
    render() {
        return (new native_ui_1.VStack(new native_ui_1.HStack(new native_ui_1.LocalImage('/home/aurelle/Pictures/peepobank/peepo-glod.png').size(40), new native_ui_1.SearchInput({ placeholder: 'la ting la bing', style: "font-size: 16px", onTextChanged: OnSearch }), new native_ui_1.LocalImage('/home/aurelle/peepo-weird-wid.gif').size(40))
            .margins(0), new FruitList())
            .margins(10));
    }
}
;
const main = () => __awaiter(void 0, void 0, void 0, function* () {
    const app = new Application();
    const client = new omnicast_1.OmnicastClient(app);
    client.render();
});
main();
