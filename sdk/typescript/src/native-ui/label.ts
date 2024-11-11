import { NativeElement } from './types';

export type LabelProps = {
	text: string;
};

export class Label extends NativeElement<LabelProps> {
	constructor(text: string) {
		super(Label.name, { text }, []);
	}
}
