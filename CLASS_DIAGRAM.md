# Updated Class Diagram - Subtask 2

## Design Patterns Implemented

### 1. SINGLETON PATTERN
```
┌─────────────────────────────────┐
│     CentralRegistry             │
├─────────────────────────────────┤
│ - instance: CentralRegistry*    │  (static)
│ - pricingPolicy: string         │
│ - emergencyMode: bool           │
├─────────────────────────────────┤
│ - CentralRegistry()             │  (private constructor)
│ + getInstance(): CentralRegistry*│ (static)
│ + setPricingPolicy(string)      │
│ + getPricingPolicy(): string    │
│ + setEmergencyMode(bool)        │
│ + isEmergencyMode(): bool       │
└─────────────────────────────────┘
```

### 2. STRATEGY PATTERN
```
                ┌──────────────────────────┐
                │   <<interface>>          │
                │   PricingStrategy        │
                ├──────────────────────────┤
                │ + calculatePrice()       │
                │ + getName(): string      │
                └──────────────────────────┘
                           △
                           │ (implements)
          ┌────────────────┼────────────────┐
          │                │                │
┌─────────────────┐ ┌──────────────┐ ┌─────────────────┐
│StandardPricing  │ │DiscountedP..  │ │EmergencyPricing │
├─────────────────┤ ├──────────────┤ ├─────────────────┤
│+ calculatePrice │ │+calculatePrice│ │+ calculatePrice │
│  returns 100%   │ │  returns 90%  │ │  returns 50%    │
│+ getName()      │ │+ getName()    │ │+ getName()      │
└─────────────────┘ └──────────────┘ └─────────────────┘
```

### 3. COMMAND PATTERN
```
        ┌──────────────────────────┐
        │   <<interface>>          │
        │      Command             │
        ├──────────────────────────┤
        │ + execute(): bool        │
        │ + getLog(): string       │
        └──────────────────────────┘
                   △
                   │ (implements)
                   │
        ┌──────────────────────────┐
        │   PurchaseCommand        │
        ├──────────────────────────┤
        │ - product: Product*      │
        │ - quantity: int          │
        │ - strategy: PricingStr*  │
        │ - finalPrice: double     │
        ├──────────────────────────┤
        │ + execute(): bool        │
        │ + getLog(): string       │
        └──────────────────────────┘
```

## Supporting Classes

### Product Class
```
┌─────────────────────────────────┐
│         Product                 │
├─────────────────────────────────┤
│ - id: string                    │
│ - name: string                  │
│ - basePrice: double             │
│ - stock: int                    │
├─────────────────────────────────┤
│ + Product(id, name, price, stk) │
│ + getId(): string               │
│ + getName(): string             │
│ + getBasePrice(): double        │
│ + getStock(): int               │
│ + setStock(int)                 │
│ + canPurchase(qty): bool        │
└─────────────────────────────────┘
```

### Inventory Class
```
┌─────────────────────────────────┐
│         Inventory               │
├─────────────────────────────────┤
│ - products: map<string, Product*>│
├─────────────────────────────────┤
│ + addProduct(Product*)          │
│ + getProduct(id): Product*      │
│ + display()                     │
└─────────────────────────────────┘
```

## Relationships

```
PurchaseCommand ──uses──> Product
PurchaseCommand ──uses──> PricingStrategy
Inventory ──contains──> Product (1 to many)
Main ──uses──> CentralRegistry (Singleton)
Main ──uses──> Inventory
Main ──uses──> PricingStrategy variants
Main ──creates──> PurchaseCommand
```

## Pattern Interactions in Simulation

1. **Singleton (CentralRegistry)** stores global pricing policy
2. **Strategy (PricingStrategy)** calculates prices based on policy
3. **Command (PurchaseCommand)** executes purchase using selected strategy
4. All three work together to enable **dynamic pricing changes at runtime**

## Legend
- `<<interface>>` : Abstract class with pure virtual functions
- `△` : Inheritance/Implementation
- `──uses──>` : Dependency relationship
- `──contains──>` : Composition relationship
