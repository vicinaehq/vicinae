import { OmnicastClient, UpdateOptions } from './omnicast';
import { Action, Container, CurrentRowChangedAction, List, ListItem, SearchInput, StatefulComponent, TextChangedAction } from './ui';

class OnSearch extends TextChangedAction {}
class OnRowChanged extends CurrentRowChangedAction {}
class TimeoutOkay extends Action {}

class Application extends StatefulComponent {
	selected: number = 0
	inputValue = ""
	count = 0;
	items = [
		{name: 'banana'},
		{name: 'strawberry'},
		{name: 'blueberry'},
		{name: 'orange'},
		{name: 'abricot'},
		{name: 'pineapple'},
	]
	activeItem = this.items[0]

	update(action: OnSearch | OnRowChanged | TimeoutOkay, { dispatchAsync }: UpdateOptions) {
		if (action instanceof OnSearch) {
			this.inputValue = action.value;
		}
		else if (action instanceof OnRowChanged) {
			this.selected = action.value;
			this.activeItem = this.items[action.value];
			console.log('dispatching');
			dispatchAsync(async () => {
				return new Promise<TimeoutOkay>(resolve => setTimeout(() => resolve(new TimeoutOkay()), 2000));
			});
		}
		else if (action instanceof TimeoutOkay) {
			console.log('timeout is okay');
		} else {
			
		}


		++this.count;
	}

	render() {
		console.log({ item: this.activeItem });

		return (
			new Container(
				[
					new SearchInput({ placeholder: 'la ting la bing', onTextChanged: OnSearch }),
					new List(
						this.items.map(({ name }) => new ListItem({ label: name })),
						{ selected: this.selected, currentRowChanged: OnRowChanged  }
					)
				],
				{ direction: 'vertical', margins: 10 }
			)
		);
	}
};



const main = async () => {
	const app = new Application();
    const client = new OmnicastClient(app);

	client.render();
}

main();
