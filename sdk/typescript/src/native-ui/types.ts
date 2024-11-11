import { randomUUID } from "crypto";
import { UpdateOptions } from "../omnicast";
import { Alignment } from "./container";

export class Action {};

export type EventHandler = Function;

export type Node = Component | NativeElement;

export class NativeElement<T = Record<string, any>> {
	id = Symbol('')
	constructor(public type: string, public props: T, public children: Node[]) {}
	
	buildProp(key: keyof T, value: T[keyof T]) {
		this.props[key] = value;
		return this;
	}

	selfAlign(align: Alignment) {
		(this.props as any).selfAlign = align;
		return this;
	}
};

export type SearchInputProps = {
	onTextChanged?: ActionFactory<TextChangedAction>;
	placeholder?: string;
	style?: string;
};

export class SearchInput extends NativeElement<SearchInputProps> {
	constructor(props: SearchInputProps) {
		super(SearchInput.name, props, []);
	}
};


export abstract class Component {
	id = Symbol(randomUUID())
	type = this.constructor.name
	props: Record<string, any> = {};
	children: Node[] = []

	abstract onMount(): void;
	abstract update(action: Action, options: UpdateOptions): void;
	abstract render(): NativeElement;
};


export const HandlerTypes = [
	'onTextChanged',
	'currentRowChanged'
] as const;

export type HandlerType = typeof HandlerTypes[number];

export const isHandlerType = (s: string): s is HandlerType => HandlerTypes.includes(s as any);

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


