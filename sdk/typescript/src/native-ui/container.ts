import { NativeElement, Node } from './types';

export const Alignments = ['left', 'top', 'bottom', 'right', 'center', 'hcenter', 'vcenter'] as const;
export type Alignment = typeof Alignments[number];

export type ContainerProps = {
	direction?: 'horizontal' | 'vertical'
	margins?: [number, number, number, number] | number;
	style?: string;
	spacing?: number;
	align?: Alignment;
}

export class Container extends NativeElement<ContainerProps> {
	constructor(children: Node[], props: ContainerProps) {
		super('container', props, children);
	}

	margins(n: number | [number, number, number, number]): Container {
		return this.buildProp('margins', n);
	}

	style(s: string) { return this.buildProp('style', s); }

	spacing(n: number) { return this.buildProp('spacing', n); }

	align(align: Alignment) { return this.buildProp('align', align); }
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
