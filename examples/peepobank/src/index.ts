import { Component, CurrentRowChangedAction, HStack, Image, KeyPressAction, Label, List, ListItem, LocalImage, OmnicastClient, SearchInput, TextChangedAction, VStack } from '@omnicast/sdk';
import { readdirSync } from 'fs';
import { join } from 'path';

export class OnSearchChanged extends TextChangedAction {};
export class OnInputKeyPress extends KeyPressAction {};
export type AppAction = OnSearchChanged | OnInputKeyPress;

export type Peepo = {
	name: string;
	path: string;
};


class PeepoRow extends Component {
	constructor(private readonly peepo: Peepo) {
		super();
	}

	onMount() {
	}

	update(action: AppAction) {
	}

	render() {
		return new HStack(
			new HStack(
				new LocalImage(this.peepo.path).height(40),
				new Label(this.peepo.name)
			)
			.selfAlign('left'),
			new Label("TYPE")
			.selfAlign('right')
		).margins(4);
	}
}

class DetailView extends Component {
	constructor(private readonly peepo: Peepo) {
		super();
	}

	onMount() {
	}

	update(action: AppAction) {
	}

	render() {
		return new VStack(
			new HStack(),
			new LocalImage(this.peepo.path).height(100).selfAlign('center'),
			new HStack(),
		).stretch(2)
	}
}

class OnPeepoRowChanged extends CurrentRowChangedAction {
}

class Application extends Component {
	private peepos: Peepo[] = []
	private selectedPeepos: Peepo[] = [];
	private selectedIdx = 0;

	constructor(private readonly bank: string) {
		super();
	}

	onMount() {
		for (const name of readdirSync(this.bank)) {

			this.peepos.push({
				name,
				path: join(this.bank, name)
			});
		}
	}

	search(q: string) {
		this.selectedPeepos = [];

		if (!q) { 
			this.selectedPeepos = [];
			return;
		}

		for (const peepo of this.peepos) {
			if (!peepo.name.includes(q)) continue ;

			this.selectedPeepos.push(peepo);
			
			if (this.selectedPeepos.length > 50) break ;
		}

		this.selectedIdx = this.selectedPeepos.length > 0 ? 0 : -1;
	}

	handleKeyPress(code: string) {
		if (code == "Up") {
			this.selectedIdx = Math.max(0, this.selectedIdx - 1);
		}
		else if (code == "Down") {
			this.selectedIdx = Math.min(this.selectedIdx + 1, this.selectedPeepos.length);
		}
	}

	update(action: AppAction) {
		if (action instanceof OnSearchChanged) {
			this.search(action.value);
		}
		if (action instanceof OnPeepoRowChanged) {
			console.log({ action });
			this.selectedIdx = action.value == -1 ? 0 : action.value;
		}
		if (action instanceof OnInputKeyPress) {
			this.handleKeyPress(action.key);
		}
	}

	render() {
		const selectedPeepo = this.selectedPeepos[this.selectedIdx];

		console.log(selectedPeepo);

		return new VStack(
			new HStack(
				new SearchInput()
				.focused(true)
				.placeholder("Search for a peepo")
				.onTextChanged(OnSearchChanged)
				.onKeyPressed(OnInputKeyPress)
			),
			new HStack(
				new List(
					...this.selectedPeepos.map(peepo => new ListItem(new PeepoRow(peepo)))
				)
				.selected(this.selectedIdx)
				.currentRowChanged(OnPeepoRowChanged)
				.verticalScrollBar('always-off')
				.horizontalScrollBar('always-off')
				.stretch(1),
				selectedPeepo && new DetailView(selectedPeepo),
			)
		)
		.margins(10)
	}
};

const main = async () => {
	const client = new OmnicastClient(new Application("/home/aurelle/Pictures/peepobank/"));

	client.render();
}

main();
