import { NativeElement, Node } from './types';

export type ContainerProps = {
	direction?: 'horizontal' | 'vertical'
	margins?: [number, number, number, number] | number;
	style?: string;
};

export class Container extends NativeElement<ContainerProps> {
	constructor(children: Node[], props: ContainerProps) {
		super('container', props, children);
	}

	margins(n: number | [number, number, number, number]): Container {
		return this.buildProp('margins', n);
	}

	style(s: string) { return this.buildProp('style', s); }
}

export class VStack extends Container {
	constructor(...children: Node[]) {
		super(children, { margins: 0, direction: 'vertical' });
	}
}

export class HStack extends Container {
	constructor(...children: Node[]) {
		super(children, { margins: 0, direction: 'horizontal' });
	}
}
