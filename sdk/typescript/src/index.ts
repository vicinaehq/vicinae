import { OmnicastClient } from './omnicast';
import { Action, CurrentRowChangedAction, HStack, List, ListItem, LocalImage, SearchInput, Component, TextChangedAction, VStack, Label } from './native-ui';

class OnSearch extends TextChangedAction {}
class OnRowChanged extends CurrentRowChangedAction {}
class TimeoutOkay extends Action {}

class PeepoRow extends Component {
	onMount() {
	}

	update() {}

	render() {
		return (
			new HStack(
				new HStack(
					new LocalImage('/home/aurelle/Pictures/peepobank/peepo-glod.png').height(40),
					new Label(`Ting ${Date.now()}`)
				).selfAlign('left'),
				new Label('TYPE').selfAlign('right')
			).margins([5, 10, 10, 5])
		);
	}
}

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
				...Array.from({ length: 150 }).fill(0).map(() => new ListItem(new PeepoRow())),
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
				//.style("background-color: green")
				.margins(0),
				new FruitList(),
				new HStack(
					new LocalImage('/home/aurelle/Pictures/peepobank/peepo-glod.png').width(40),
					new SearchInput({ placeholder: 'la ting la bing', style: "font-size: 16px", onTextChanged: OnSearch }),
					new LocalImage('/home/aurelle/peepo-weird-wid.gif').width(40)
				)
				//.style("background-color: red"),
			)
			.margins(10)
			//.style("background-color: blue")
		);
	}
};

class Application2 extends Component {
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
			new HStack(
				new Label('left').selfAlign('left'),
				new Label('right').selfAlign('right')
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
