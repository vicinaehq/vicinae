import { OmnicastClient } from './omnicast';
import { Action, CurrentRowChangedAction, HStack, List, ListItem, LocalImage, SearchInput, Component, TextChangedAction, VStack } from './native-ui';

class OnSearch extends TextChangedAction {}
class OnRowChanged extends CurrentRowChangedAction {}
class TimeoutOkay extends Action {}

class FruitList extends Component {
	items = [
		{name: 'banana'},
		{name: 'strawberry'},
		{name: 'blueberry'},
		{name: 'orange'},
		{name: 'abricot'},
		{name: 'pineapple'},
	]
	activeItem = this.items[0]
	selected = 0

	onMount() {
		console.log(`mounted ${this.type}`);
	}

	update(action: Action) {
		if (action instanceof OnRowChanged) {
			this.selected = action.value;
			console.log({ selected: this.selected });
		}
	}

	render() {
		console.log(`render list: ${this.selected}`);
		return (
			new List(
				...this.items.map(({ name }) => new ListItem(name)),
			)
			.selected(this.selected)
			.currentRowChanged(OnRowChanged)
		)
	}
};

class Application extends Component {
	inputValue = ""
	count = 0;
	
	onMount() {
		console.log('mounted');
	}

	updateProps() {
	}

	update(action: OnSearch | OnRowChanged | TimeoutOkay) {
		if (action instanceof OnSearch) {
			this.inputValue = action.value;
		}

		++this.count;
	}

	render() {
		return (
			new VStack(
				new HStack(
					new LocalImage('/home/aurelle/Pictures/peepobank/peepo-glod.png').size(40),
					new SearchInput({ placeholder: 'la ting la bing', style: "font-size: 16px", onTextChanged: OnSearch }),
					new LocalImage('/home/aurelle/peepo-weird-wid.gif').size(40)
				)
				.margins(0),
				new FruitList()
			)
			.margins(10)
		);
	}
};



const main = async () => {
	const app = new Application();
    const client = new OmnicastClient(app);

	client.render();
}

main();
