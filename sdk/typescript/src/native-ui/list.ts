import { ActionFactory, CurrentRowChangedAction, Node, NativeElement } from './types';

export type ListProps = {
	selected?: number;
	currentRowChanged?: ActionFactory<CurrentRowChangedAction>;
	stretch?: 0 | 1;
};

export class List extends NativeElement<ListProps> {
	constructor(...children: ListItem[]) {
		super(List.name, {}, children);
	}
	
	selected(n: number) { return this.buildProp('selected', n) }
	stretch(n: number) { return this.buildProp('stretch', n); }
	
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
