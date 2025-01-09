namespace DataApi
{
    public record Invoice(DateOnly Date, IList<InvoiceLine> InvoiceLines)
    {
    }
}
