const values = ['development', 'production'] as const

export type EnvironmentType = typeof values[number];
