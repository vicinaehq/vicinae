import { ActionFactory, NativeElement, TextChangedAction } from "./types";

export type SearchInputProps = {
	onTextChanged?: ActionFactory<TextChangedAction>;
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

	placeholder(s: string) {
		return this.buildProp('placeholder', s);
	}

	focused(b: boolean) {
		return this.buildProp('focused', b);
	}
};
