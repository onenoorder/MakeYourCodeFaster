namespace DataApi
{
    public record InvoicesViewModel(DateOnly FromDate, DateOnly ToDate, int Count, IEnumerable<Invoice> Invoices)
    {
    }
}
