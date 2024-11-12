import { ActionFactory, KeyPressAction, NativeElement, TextChangedAction } from "./types";


export type SearchInputProps = {
	onTextChanged?: ActionFactory<TextChangedAction>;
	onKeyPress?: ActionFactory<KeyPressAction>;
	placeholder?: string;
	style?: string;
	focused?: boolean;
};

export class SearchInput extends NativeElement<SearchInputProps> {
	constructor() {
		super(SearchInput.name, {}, []);
	}

	onTextChanged(action: ActionFactory<TextChangedAction>) {
		return this.buildProp('onTextChanged', action);
	}

    onKeyPressed(action: ActionFactory<KeyPressAction>) {
		return this.buildProp('onKeyPress', action);
	}

	placeholder(s: string) {
		return this.buildProp('placeholder', s);
	}

	focused(b: boolean) {
		return this.buildProp('focused', b);
	}
};
