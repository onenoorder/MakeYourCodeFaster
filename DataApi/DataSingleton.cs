namespace DataApi
{
    public class DataSingleton
    {
        private IList<Invoice> _invoices;

        public DataSingleton() {
            _invoices = new List<Invoice>();
            GenerateInvoices();
        }

        private void GenerateInvoices() {
            var today = new DateOnly(2024, 1, 1);
            var currentInvoiceDay = new DateOnly(today.Year, today.Month, 1);
            Random random = new Random();

            while (currentInvoiceDay.Year == today.Year) {
                for (int index = 0; index <= random.Next(100, 1000); index++) {
                    _invoices.Add(new Invoice(
                        currentInvoiceDay,
                        Enumerable.Range(1, random.Next(1, 10)).Select(test => new InvoiceLine(random.Next(10, 1000))).ToList()
                    ));
                }

                currentInvoiceDay = currentInvoiceDay.AddDays(1);
            }
        }

        internal IList<Invoice> GetInvoices() => _invoices.ToList();
    }
}
