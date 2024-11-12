import { ActionFactory, CurrentRowChangedAction, Node, NativeElement } from './types';

export const ScrollbarPolicies = [
	'always-on',
	'always-off',
	'auto'
] as const;

export type ScrollbarPolicy = typeof ScrollbarPolicies[number];

export type ListProps = {
	selected?: number;
	currentRowChanged?: ActionFactory<CurrentRowChangedAction>;
	stretch?: number;
	hscroll?: ScrollbarPolicy;
	vscroll?: ScrollbarPolicy;
};

export class List extends NativeElement<ListProps> {
	constructor(...children: ListItem[]) {
		super(List.name, {}, children);
	}
	
	selected(n: number) { return this.buildProp('selected', n) }
	stretch(n: number) { return this.buildProp('stretch', n); }
	horizontalScrollBar(s: ScrollbarPolicy) { return this.buildProp('hscroll', s); }
	verticalScrollBar(s: ScrollbarPolicy) { return this.buildProp('vscroll', s); }
	
	currentRowChanged(action: ActionFactory<CurrentRowChangedAction>) { return this.buildProp('currentRowChanged', action); }
}

export type ListItemProps = {
	label?: string;
	selected?: boolean;
};

export class ListItem extends NativeElement<ListItemProps> {
	constructor(name: string | Node) {
		if (typeof name == 'string') {
			super(ListItem.name, { label: name }, []);
			return ;
		}

		super(ListItem.name, {}, [name]);
	}
}
