import { UpdateOptions } from "./omnicast";

export const HandlerTypes = [
	'onTextChanged',
	'currentRowChanged'
] as const;

export class Action {};

export type TextChangedActionPayload = {
	value: string;
};

export type CurrentRowChangedActionPayload = {
	value: number;
};

export class CurrentRowChangedAction {
	value: number;

	constructor(init: CurrentRowChangedActionPayload) {
		this.value = init.value;
	}
};

export class TextChangedAction {
	value: string;

	constructor(init: TextChangedActionPayload) {
		this.value = init.value;
	}
};

export type ActionFactory<T extends Action> = new (...args: any[]) => T;

export type HandlerType = typeof HandlerTypes[number];

export const isHandlerType = (s: string): s is HandlerType => HandlerTypes.includes(s as any);

export type EventHandler = Function;

export class Component<T = Record<string, any>> {
	constructor(public type: string, public props: T, public children: (Component | StatefulComponent)[]) {}
};

export type ListProps = {
	selected?: number;
	currentRowChanged?: ActionFactory<CurrentRowChangedAction>;
};

export class List extends Component {
	constructor(children: ListItem[], props: ListProps = {}) {
		super(List.name, props, children);
	}
}

export type ListItemProps = {
	label: string;
	selected?: boolean;
};



export class ListItem extends Component<ListItemProps> {
	constructor(props: ListItemProps) {
		super(ListItem.name, props, []);
	}
}

export type SearchInputProps = {
	onTextChanged?: ActionFactory<TextChangedAction>;
	placeholder?: string;
	style?: string;
};

export class SearchInput extends Component<SearchInputProps> {
	constructor(props: SearchInputProps) {
		super(SearchInput.name, props, []);
	}
};

export type ContainerProps = {
	direction?: 'horizontal' | 'vertical'
	margins?: [number, number, number, number] | number;
};

export class Container extends Component<ContainerProps> {
	constructor(children: Component[], props: ContainerProps) {
		super('container', props, children);
	}
}

export class VStack extends Container {
	constructor(...children: Component[]) {
		super(children, { margins: 0, direction: 'vertical' });
	}
}

export abstract class StatefulComponent {
	abstract update(action: Action, options: UpdateOptions): void;
	abstract render(): Component;
};
